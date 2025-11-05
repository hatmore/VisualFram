from __future__ import annotations

import json
import logging
import os
import platform
import sys
import traceback
from threading import Thread
from typing import TYPE_CHECKING, Any, Callable, Dict, cast

from logistro import _args as cli_args

if TYPE_CHECKING:
    from collections.abc import MutableMapping

## New Constants and Globals

DEBUG2 = 5
"""A more verbose version of `logging.DEBUG`"""

logging.addLevelName(DEBUG2, "DEBUG2")

pipe_attr_blacklist = ["filename", "funcName", "threadName", "taskName"]
"""List of attributes to ignore in getPipeLogger()"""

# Our basic formatting list
_output = {
    "time": "%(asctime)s",
    "level": "%(levelname)s",
    "name": "%(name)s",
    "file": "%(filename)s",
    "func": "%(funcName)s",
    "task": "%(taskName)s",
    "thread": "%(threadName)s",
    "message": "%(message)s",
}

# A more readable, human readable string
_date_string = "%a, %d-%b %H:%M:%S"

# async taskName not supported below 3.12, remove it
if bool(sys.version_info[:3] < (3, 12)):
    del _output["task"]

# Make human output a little more readable
_output_human = _output.copy()
_output_human["func"] += "()"
_output_human["time"] = ""


class HumanFormatter(logging.Formatter):
    def format(self, record: logging.LogRecord) -> str:
        # level name
        result: str = record.levelname + "\t"
        thread = None if record.threadName == "MainThread" else record.threadName
        if thread:
            result += f"Thread({thread}) "
        task = record.taskName if hasattr(record, "taskName") else None
        if task:
            result += f"Task({task}) "
        mod = record.name or ""
        filename = record.filename or ""
        result += f"{mod}:{filename}"
        func = record.funcName or None
        if func:
            result += f":{func}()"
        message = str(record.msg) % record.args if record.args else str(record.msg)
        result += f"- {message}"
        if record.exc_info:
            result += f"\n {' '.join(traceback.format_exception(*record.exc_info))}"

        return result


human_formatter = HumanFormatter()

structured_formatter = logging.Formatter(json.dumps(_output))
"""A `logging.Formatter()` to print output as JSON for machine consumption."""


# its possible that the user has already changed the base class.


# https://github.com/python/mypy/wiki/Unsupported-Python-Features
class _LogistroLogger(logging.getLoggerClass()):  # type: ignore[misc]
    def debug1(self, msg: str, *args: Any, **kwargs: Any) -> None:
        stacklevel = kwargs.pop("stacklevel", 0) + 2
        super().log(logging.DEBUG, msg, *args, stacklevel=stacklevel, **kwargs)

    def debug2(self, msg: str, *args: Any, **kwargs: Any) -> None:
        stacklevel = kwargs.pop("stacklevel", 0) + 2
        super().log(DEBUG2, msg, *args, stacklevel=stacklevel, **kwargs)


logging.setLoggerClass(_LogistroLogger)


def set_human() -> None:
    """Set `--logistro-human` (default)."""
    cli_args.parsed.human = True


def set_structured() -> None:
    """Set `--logistro-structured`."""
    cli_args.parsed.human = False


def coerce_logger(
    logger: logging.Logger,
    formatter: logging.Formatter | None = None,
) -> None:
    """
    Set all a logger's formatters to the formatter specified or default.

    Args:
        logger: The logger to coerce
        formatter: The `logging.Formatter()` object to use- defaults to
            human_formatter or structured_formatter.

    """
    if not formatter:
        formatter = human_formatter if cli_args.parsed.human else structured_formatter
    for handler in logger.handlers:
        handler.setFormatter(formatter)


def betterConfig(**kwargs: Any) -> None:  # noqa: N802 camel-case like logging
    """
    Call `logging.basicConfig()` with our defaults.

    It will overwrite any `format` or `datefmt` arguments passed.
    It is only ever run once.
    """
    implicit = kwargs.pop("implicit", False)
    # its implicitly called and we're already setup
    if not implicit or not logging.getLogger().handlers:
        if "level" not in kwargs and cli_args.parsed.log:
            if cli_args.parsed.log.isnumeric():
                kwargs["level"] = int(cli_args.parsed.log)
            else:
                kwargs["level"] = cli_args.parsed.log.upper()
        logging.basicConfig(**kwargs)
        coerce_logger(logging.getLogger())
    betterConfig.__code__ = (lambda **_kwargs: None).__code__
    # function won't run after this


def getLogger(name: str | None = None) -> _LogistroLogger:  # noqa: N802 camel-case like logging
    """Call `logging.getLogger()` but check `betterConfig()` first."""
    betterConfig(implicit=True)
    return cast("_LogistroLogger", logging.getLogger(name))


_LoggerFilter = Callable[[logging.LogRecord, Dict[str, Any]], bool]


class _PipeLoggerFilter:
    _parser: _LoggerFilter | None

    def __init__(
        self,
        parser: _LoggerFilter | None,
    ) -> None:
        self._parser = parser

    def filter(self, record: logging.LogRecord) -> bool:
        old_info = {}
        for attr in pipe_attr_blacklist:
            if hasattr(record, attr):
                old_info[attr] = getattr(record, attr)
                setattr(record, attr, "")
        if self._parser:
            return self._parser(record, old_info)
        return True


FD = int


def getPipeLogger(  # noqa: N802 camel-case like logging
    name: str,
    parser: _LoggerFilter | None = None,
    default_level: int = logging.DEBUG,
    ifs: str | bytes = "\n",
) -> tuple[FD, _LogistroLogger]:
    r"""
    Is a special `getLogger()` which returns a pipe and logger.

    It spins a separate thread to feed the pipe into the logger.
    Common usage would be to pass the pipe to `Popen(stderr=...)`.
    You should close the pipe `os.close(pipe)` in case the other process
    freezes.

    Args:
        name: the name of the logger (á la `logging.getLogger(name)`)
        parser: a function whose first parameter is a `LogRecord` to modify it.
            If it doesn't return `True`, the whole record is ignored.
            The second parameter is a dictionary of already filtered info.
            See `pipe_attr_blacklist` in `custom_logging.py`.
        default_level: the default level for the logger.
        ifs: The character used as delimiter for pipe reads, defaults:"\n".

    Returns:
        A tuple: (pipe, logger)

    """
    ifs = ifs.encode("utf-8") if isinstance(ifs, str) else ifs
    logger = getLogger(name)
    logger.addFilter(_PipeLoggerFilter(parser))
    r, w = os.pipe()

    # needs r, logger, and default_level
    def read_pipe(
        r: int,
        logger: logging.Logger,
        default_level: int,
    ) -> None:
        if bool(sys.version_info[:3] >= (3, 12) or platform.system() != "Windows"):
            os.set_blocking(r, True)
        raw_buffer = b""
        while True:
            try:
                last_size = len(raw_buffer)
                raw_buffer += os.read(
                    r,
                    1000,
                )
                if last_size == len(raw_buffer):
                    # Just move us to exception mode
                    raise Exception("")  # noqa: TRY002,TRY301
            # Catch any exception, its all weird OS stuff, the game is up
            except Exception:  # noqa: BLE001
                while raw_buffer:
                    line, _, raw_buffer = raw_buffer.partition(ifs)
                    if line:
                        logger.log(default_level, line.decode())
                return
            while raw_buffer:
                line, m, raw_buffer = raw_buffer.partition(ifs)
                if not m:
                    raw_buffer = line
                    break
                logger.log(default_level, line.decode())

    pipe_reader = Thread(
        target=read_pipe,
        name=name + "Thread",
        args=(r, logger, default_level),
    )
    pipe_reader.start()
    return w, logger


class LoggingNode:
    def __init__(
        self,
        name: str,
        *,
        logger: logging.Logger | None = None,
        parent: LoggingNode | None = None,
    ) -> None:
        self.logger = logger
        self._name = name
        self.children: MutableMapping[str, LoggingNode] = {}
        self.parent = parent

    def add_child(self, name: str, logger: logging.Logger | None = None) -> LoggingNode:
        if logger:
            if name in self.children:
                self.children[name].logger = logger
            else:
                self.children[name] = LoggingNode(name, logger=logger, parent=self)
        else:
            self.children[name] = LoggingNode(name, parent=self)
        return self.children[name]

    def print(self, indent: int = 0) -> None:
        label = f"{self._name}- {self.logger}"
        if self.logger and hasattr(self.logger, "handlers"):
            label += f"- {self.logger.handlers}"
        print(("-" * indent) + label)  # noqa: T201
        for _, child in sorted(self.children.items()):
            child.print(indent + 1)

    def __str__(self) -> str:
        return self._name


def describe_logging() -> None:
    """Print out information about the current logging configuration."""
    logger = getLogger()
    root = LoggingNode("root", logger=logger, parent=None)
    for name, sublogger in logger.manager.loggerDict.items():
        parent = root
        nodes = name.split(".")
        for node in nodes[:-1]:
            parent = parent.add_child(node)
        parent.add_child(nodes[-1], sublogger)

    root.print()
