{
    "ConmmDevice": {
        "CommunicateDevices": {
            "IP": "127.0.0.1",
            "Name": "TcpServer",
            "OpterateArray": [
                {
                    "Name": "sendDataComplete",
                    "Type": "Int"
                },
                {
                    "Name": "sendDataControl",
                    "Type": "Int"
                }
            ],
            "Port": 8500
        }
    },
    "Device": {
        "CamerasSet": [
            {
                "Key": 0,
                "Name": "camera2",
                "URL": "G:\\\\PengSiyao\\\\dataset\\\\014.mp4",
                "camerIndex": 0,
                "isSave": false,
                "savePath": "",
                "timeSpan": 500,
                "virtualCamera": true
            }
        ]
    },
    "Project": {
        "Project": [
            {
                "FlowGraphModel": {
                    "connections": [
                        {
                            "inPortIndex": 0,
                            "intNodeId": 6,
                            "outNodeId": 5,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 1,
                            "intNodeId": 7,
                            "outNodeId": 5,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 0,
                            "intNodeId": 7,
                            "outNodeId": 6,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 0,
                            "intNodeId": 14,
                            "outNodeId": 10,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 1,
                            "intNodeId": 9,
                            "outNodeId": 6,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 2,
                            "intNodeId": 9,
                            "outNodeId": 8,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 0,
                            "intNodeId": 14,
                            "outNodeId": 9,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 1,
                            "intNodeId": 10,
                            "outNodeId": 6,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 2,
                            "intNodeId": 10,
                            "outNodeId": 8,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 0,
                            "intNodeId": 15,
                            "outNodeId": 9,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 0,
                            "intNodeId": 10,
                            "outNodeId": 16,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 0,
                            "intNodeId": 15,
                            "outNodeId": 10,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 0,
                            "intNodeId": 16,
                            "outNodeId": 5,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 0,
                            "intNodeId": 9,
                            "outNodeId": 16,
                            "outPortIndex": 0
                        }
                    ],
                    "nodes": [
                        {
                            "id": 8,
                            "internal-data": {
                                "SerializeFunciton": {
                                    "FunctionParam": "0000002000730065006e006400440061007400610043006f006d0070006c006500740065",
                                    "UiParam": "00000000"
                                },
                                "model-name": "外部设备通讯",
                                "nodeTitle": "外部设备通讯"
                            },
                            "position": {
                                "x": -514.4159999999997,
                                "y": 76.416
                            }
                        },
                        {
                            "id": 10,
                            "internal-data": {
                                "SerializeFunciton": {
                                    "FunctionParam": "400800000000000000000003403e000000000000",
                                    "UiParam": "00000000"
                                },
                                "model-name": "持续时间",
                                "nodeTitle": "持续时间"
                            },
                            "position": {
                                "x": -152.17785433908716,
                                "y": -123.39221499704334
                            }
                        },
                        {
                            "id": 5,
                            "internal-data": {
                                "SerializeFunciton": {
                                    "FunctionParam": "00000000000000380047003a002f00500065006e00670053006900790061006f002f003000310034005f003000300030003200360034002e006a007000650067ffffffff00000018003100390032002e0031003600380030002e0030002e00300000000e00630061006d0065007200610032000007d0",
                                    "UiParam": "00000000"
                                },
                                "model-name": "图像源",
                                "nodeTitle": "图像源"
                            },
                            "position": {
                                "x": -837.5099692212984,
                                "y": -64.83685127731607
                            }
                        },
                        {
                            "id": 6,
                            "internal-data": {
                                "SerializeFunciton": {
                                    "FunctionParam": "0000000d000000040000000d000000000000000100000001000000020000000200000002000000020000000200000003000000030000000300000003000000030000000d000000045de54ef600000006624b90e8003100000006624b90e8003200000006624e5e26003100000006624e5e26003200000006624e5e26003300000006624e5e26003400000006624e5e26003500000008526a624e5e26003100000008526a624e5e26003200000008526a624e5e26003300000008526a624e5e26003400000008526a624e5e2600350000007a0044003a002f00560069007300750061006c004600720061006d0032002e003000490049002f00560069007300750061006c004600720061006d00320044002f004d006f00640065006c002f0079006f006c006f00310031006d005f006400650074006500630074005f003600340030002e0070006c0061006e00000280000002803fe0000000000000000000040000001c00650071007500690070006d0065006e0074005300680065006c006c0000000800680061006e00640000000c0072006900620062006f006e000000120063007500740052006900620062006f006e",
                                    "UiParam": "00000000"
                                },
                                "model-name": "目标检测模型",
                                "nodeTitle": "目标检测模型"
                            },
                            "position": {
                                "x": -538.7656696632416,
                                "y": -91.63052539383519
                            }
                        },
                        {
                            "id": 14,
                            "internal-data": {
                                "SerializeFunciton": {
                                    "FunctionParam": "00000001000000660044003a002f00560069007300750061006c004600720061006d0032002e003000490049002f00560069007300750061006c004600720061006d00320044002f0050007900740068006f006e002f0053004f0050005200650070006f00720074002e00700079",
                                    "UiParam": "00000000"
                                },
                                "model-name": "SOP报表",
                                "nodeTitle": "SOP报表"
                            },
                            "position": {
                                "x": 195.1472,
                                "y": -85.1929812571031
                            }
                        },
                        {
                            "id": 7,
                            "internal-data": {
                                "SerializeFunciton": {
                                    "FunctionParam": "00000000",
                                    "UiParam": "00000000"
                                },
                                "model-name": "图像显示",
                                "nodeTitle": "图像显示"
                            },
                            "position": {
                                "x": -162.84799999999996,
                                "y": -503.0319999999999
                            }
                        },
                        {
                            "id": 15,
                            "internal-data": {
                                "SerializeFunciton": {
                                    "FunctionParam": "000000800044003a002f00560069007300750061006c004600720061006d0032002e003000490049002f00560069007300750061006c004600720061006d00320044002f0050007900740068006f006e002f0043006f006d006d0075006e0069006300610074006500440065007600690063006500570072006900740065002e00700079",
                                    "UiParam": "00000000"
                                },
                                "model-name": "写外部通讯",
                                "nodeTitle": "写外部通讯"
                            },
                            "position": {
                                "x": 190.80799999999996,
                                "y": -401.89599999999984
                            }
                        },
                        {
                            "id": 9,
                            "internal-data": {
                                "SerializeFunciton": {
                                    "FunctionParam": "0000000000000000",
                                    "UiParam": "00000000"
                                },
                                "model-name": "动作顺序",
                                "nodeTitle": "动作顺序"
                            },
                            "position": {
                                "x": -154.61600000000004,
                                "y": -323.7439999999999
                            }
                        },
                        {
                            "id": 16,
                            "internal-data": {
                                "SerializeFunciton": {
                                    "FunctionParam": "0000000b0000034d0000019d00000365000002bc000004be000002aa0000003c00000036000004c90000024e0000003a00000038000005d7000002440000003a000000360000058c0000020300000032000000340000050f000001fd000000360000002d000004ab00000294000000420000003e000004b8000002410000004200000038000004fd000001eb0000003f000000380000057c000001f4000000410000003b000005c800000234000000420000003d0000000b00000000000000020000000200000002000000020000000200000003000000030000000300000003000000030000000b000000000000000100000002000000030000000400000005000000060000000700000008000000090000000a0000000000000000000000010000000100000000",
                                    "UiParam": "00000000"
                                },
                                "model-name": "图像ROI",
                                "nodeTitle": "图像ROI"
                            },
                            "position": {
                                "x": -525.6000000000001,
                                "y": -254.40000000000003
                            }
                        }
                    ]
                },
                "TaskFlowEnabledState": true,
                "TaskFlowId": 1,
                "TaskFlowName": "flow"
            }
        ]
    }
}
