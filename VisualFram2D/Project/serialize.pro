{
    "ConmmDevice": {
        "CommunicateDevices": {
            "IP": "127.0.0.1",
            "Name": "TcpServer",
            "OpterateArray": [
                {
                    "Name": "sendDataControl",
                    "Type": "Int"
                },
                {
                    "Name": "sendDataComplete",
                    "Type": "Int"
                }
            ],
            "Port": 8600
        }
    },
    "Device": {
        "CamerasSet": [
            {
                "Key": 0,
                "Name": "camera1",
                "URL": "E:\\\\DataSet\\\\JinKangDataSet\\\\videos\\\\072204.mp4",
                "camerIndex": 0,
                "isSave": false,
                "savePath": "",
                "timeSpan": 100,
                "virtualCamera": false
            },
            {
                "Key": 1,
                "Name": "camera2",
                "URL": "E:\\\\DataSet\\\\JinKangDataSet\\\\videos\\\\072210.mp4",
                "camerIndex": 1,
                "isSave": false,
                "savePath": "",
                "timeSpan": 100,
                "virtualCamera": false
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
                            "intNodeId": 2,
                            "outNodeId": 1,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 0,
                            "intNodeId": 8,
                            "outNodeId": 1,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 0,
                            "intNodeId": 7,
                            "outNodeId": 4,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 1,
                            "intNodeId": 7,
                            "outNodeId": 9,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 0,
                            "intNodeId": 5,
                            "outNodeId": 8,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 0,
                            "intNodeId": 4,
                            "outNodeId": 8,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 2,
                            "intNodeId": 5,
                            "outNodeId": 6,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 0,
                            "intNodeId": 3,
                            "outNodeId": 2,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 2,
                            "intNodeId": 4,
                            "outNodeId": 6,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 0,
                            "intNodeId": 7,
                            "outNodeId": 5,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 1,
                            "intNodeId": 5,
                            "outNodeId": 0,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 1,
                            "intNodeId": 3,
                            "outNodeId": 1,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 1,
                            "intNodeId": 4,
                            "outNodeId": 0,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 0,
                            "intNodeId": 0,
                            "outNodeId": 1,
                            "outPortIndex": 0
                        },
                        {
                            "inPortIndex": 0,
                            "intNodeId": 3,
                            "outNodeId": 0,
                            "outPortIndex": 0
                        }
                    ],
                    "nodes": [
                        {
                            "id": 0,
                            "internal-data": {
                                "SerializeFunciton": {
                                    "FunctionParam": "00000005000000720044003a002f00560069007300750061006c004600720061006d0032002e003000490049002f00560069007300750061006c004600720061006d00320044002f004d006f00640065006c002f0079006f006c006f00310031006d005f006400650074006500630074002e0070006c0061006e00000005000000140043006c006100730073004e0061006d00650030000000140043006c006100730073004e0061006d00650031000000140043006c006100730073004e0061006d00650032000000140043006c006100730073004e0061006d00650033000000140043006c006100730073004e0061006d00650034",
                                    "UiParam": "00000000"
                                },
                                "model-name": "目标检测模型",
                                "nodeTitle": "目标检测模型"
                            },
                            "position": {
                                "x": -445.4800000000001,
                                "y": -81.96000000000001
                            }
                        },
                        {
                            "id": 1,
                            "internal-data": {
                                "SerializeFunciton": {
                                    "FunctionParam": "00000002ffffffffffffffff00000018003100390032002e0031003600380030002e0030002e00300000000e00630061006d0065007200610031",
                                    "UiParam": "00000000"
                                },
                                "model-name": "图像源",
                                "nodeTitle": "图像源"
                            },
                            "position": {
                                "x": -751.6,
                                "y": -219.20000000000002
                            }
                        },
                        {
                            "id": 2,
                            "internal-data": {
                                "SerializeFunciton": {
                                    "FunctionParam": "000000110000006e0044003a002f00560069007300750061006c004600720061006d0032002e003000490049002f00560069007300750061006c004600720061006d00320044002f004d006f00640065006c002f0079006f006c006f003100310073005f0070006f00730065002e0070006c0061006e00000011000000140043006c006100730073004e0061006d00650030000000140043006c006100730073004e0061006d00650031000000140043006c006100730073004e0061006d00650032000000140043006c006100730073004e0061006d00650033000000140043006c006100730073004e0061006d00650034000000140043006c006100730073004e0061006d00650035000000140043006c006100730073004e0061006d00650036000000140043006c006100730073004e0061006d00650037000000140043006c006100730073004e0061006d00650038000000140043006c006100730073004e0061006d00650039000000160043006c006100730073004e0061006d006500310030000000160043006c006100730073004e0061006d006500310031000000160043006c006100730073004e0061006d006500310032000000160043006c006100730073004e0061006d006500310033000000160043006c006100730073004e0061006d006500310034000000160043006c006100730073004e0061006d006500310035000000160043006c006100730073004e0061006d006500310036",
                                    "UiParam": "00000000"
                                },
                                "model-name": "关键点检测模型",
                                "nodeTitle": "关键点检测模型"
                            },
                            "position": {
                                "x": -449.9999999999999,
                                "y": -368.15999999999997
                            }
                        },
                        {
                            "id": 3,
                            "internal-data": {
                                "SerializeFunciton": {
                                    "FunctionParam": "00000000",
                                    "UiParam": "00000000"
                                },
                                "model-name": "图像显示",
                                "nodeTitle": "图像显示"
                            },
                            "position": {
                                "x": -62.14399999999996,
                                "y": -490.156
                            }
                        },
                        {
                            "id": 4,
                            "internal-data": {
                                "SerializeFunciton": {
                                    "FunctionParam": "401400000000000000000005403e000000000000",
                                    "UiParam": "00000000"
                                },
                                "model-name": "持续时间",
                                "nodeTitle": "持续时间"
                            },
                            "position": {
                                "x": -66.67999999999999,
                                "y": -116.63999999999997
                            }
                        },
                        {
                            "id": 5,
                            "internal-data": {
                                "SerializeFunciton": {
                                    "FunctionParam": "4008000000000000",
                                    "UiParam": "00000000"
                                },
                                "model-name": "动作顺序",
                                "nodeTitle": "动作顺序"
                            },
                            "position": {
                                "x": -69.11999999999999,
                                "y": -283.6799999999999
                            }
                        },
                        {
                            "id": 6,
                            "internal-data": {
                                "SerializeFunciton": {
                                    "FunctionParam": "0000002000730065006e006400440061007400610043006f006d0070006c006500740065",
                                    "UiParam": "00000000"
                                },
                                "model-name": "外部设备通讯",
                                "nodeTitle": "外部设备通讯"
                            },
                            "position": {
                                "x": -429.11999999999995,
                                "y": 95.03999999999999
                            }
                        },
                        {
                            "id": 7,
                            "internal-data": {
                                "SerializeFunciton": {
                                    "FunctionParam": "000000800044003a002f00560069007300750061006c004600720061006d0032002e003000490049002f00560069007300750061006c004600720061006d00320044002f0050007900740068006f006e002f0043006f006d006d0075006e0069006300610074006500440065007600690063006500570072006900740065002e00700079",
                                    "UiParam": "00000000"
                                },
                                "model-name": "写外部通讯",
                                "nodeTitle": "写外部通讯"
                            },
                            "position": {
                                "x": 262.0799999999999,
                                "y": -197.27999999999994
                            }
                        },
                        {
                            "id": 8,
                            "internal-data": {
                                "SerializeFunciton": {
                                    "FunctionParam": "00000005000000c100000129000001ff000001c60000011600000166000000bb000000870000011700000164000000c20000008d000000e40000013a000000a10000009a0000014600000144000000e2000000ab00000005000000000000000100000002000000030000000400000000000000000000000100000001",
                                    "UiParam": "00000000"
                                },
                                "model-name": "图像ROI",
                                "nodeTitle": "图像ROI"
                            },
                            "position": {
                                "x": -422.2079999999999,
                                "y": -219.7439999999999
                            }
                        },
                        {
                            "id": 9,
                            "internal-data": {
                                "SerializeFunciton": {
                                    "FunctionParam": "0000001e00730065006e006400440061007400610043006f006e00740072006f006c",
                                    "UiParam": "00000000"
                                },
                                "model-name": "外部设备通讯",
                                "nodeTitle": "外部设备通讯"
                            },
                            "position": {
                                "x": -57.02399999999997,
                                "y": 95.03999999999998
                            }
                        }
                    ]
                },
                "TaskFlowEnabledState": true,
                "TaskFlowId": 1,
                "TaskFlowName": "flow0"
            }
        ]
    }
}
