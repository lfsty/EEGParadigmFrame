# 范式json格式

```json
{
    "DialogSize": {},
    "MarkerInfo": [],
    "Instructions":{},
    "TrailList":{},
    "GoodBye":{},
}
```

Dialogsize:[Dialogsize](#Dialogsize)

对话框大小的配置

MarkerInfo:[打标信息配置](#打标信息配置)

包含一个数组，其中每个元素是一个标记信息对象，表示一组通信端口的配置

Instructions:[介绍信息](#介绍信息\/Instructions)

指导信息的配置

TrailList: [TrailListQobject](#TrailListQobject)

实验信息的配置

GoodBye:[结束语](#结束语/GoodBye)

结束信息的配置

## Dialogsize

```json
{"Type": "full_screen"} 
```

- Type: ***string***

全屏，表示对话框的大小类型

## 打标信息配置

### 串口

```json
{
    "Type": "COM",
    "PortName": "COM3",
    "BaudRate": 115200,
    "DataBits": 8,
    "Parity": "No",
    "StopBits": 1,
    "FlowControl": "No",
    "PreFix": "01 E1 01 00"
}
```

- Type: ***String*** , 标记的类型

- PortName: ***String*** , 通信端口的名称

- BaudRate: ***int***, 115200波特率

- DataBits: ***int*** , 8 数据位

- Parity: ***String***,  "No" 校验位

- StopBits: ***int*** , 1 停止位

- FlowControl: ***String*** , "No" 流控制

- PreFix: ***String*** , "01 E1 01 00"前缀

### TCP

~~~json
{
    "Type": "TCP",
    "IP":"",
    "Port":8000
}
~~~

- "TCP"： 表示使用TCP协议进行通信
-  "IP"：你要连接的远程主机的IP地址
- "Port"：表示通信端口号

### UDP

~~~json
{
    "Type":"UDP",
    "IP":"",
    "Port":8000
}
~~~

- "UDP"： 表示使用UDP协议进行通信
-  "IP"：你要连接的远程主机的IP地址
- "Port"：表示通信端口号

## 介绍信息/Instructions

显示模块，看[基础显示模块](#基础显示模块\/BaseDisplay)

## TrailListQobject

```json
 {
    "Round": 5,
    "LoopTimes": 40,
    "LoopMode": "random",
    "TrailProc": 
     {
         "StaticBlock": 
          {
            "Fixation":{},
            "Rest":{},
          },
         "StimulusBlock":[{ }]
         "Trail": 
       [{
            "Type": "StaticBlock",
            "Data": "Rest"
        }, {
            "Type": "StaticBlock",
            "Data": "Fixation"
        }, {
            "Type": "StimulusBlock"
        }]
    },

    "BetweenRound":{}
} 
```

- Round : ***int*** ， 5，表示本次实验轮数为5

- LoopTimes : ***int*** ， 40 表示循环次数

- LoopMode : ***String***，"random" 循环模式
- BetweenRound :  两轮之间的配置信息，显示模块，看[基础显示模块](#基础显示模块\/BaseDisplay)

####TrailProc : 

 试验过程的配置信息

-  StaticBlock ： 静息态模块配置
  - Fixation:集中注意力模块配置，显示模块，看[基础显示模块](#基础显示模块\/BaseDisplay)
  - Rest :用于实验休息模块的配置，显示模块，看[基础显示模块](#基础显示模块\/BaseDisplay)

- StimulusBlock：刺激模块的配置，显示模块，看[基础显示模块](#基础显示模块\/BaseDisplay)

- Trail ：其中包含三个QJsonObject



## 结束语/GoodBye

显示模块，看[基础显示模块](#基础显示模块\/BaseDisplay)

## 基础显示模块/BaseDisplay

~~~json
{
    "Control":{},
    "Display":[]
}
~~~

* Control: 控制模块，控制显示模块的时间，打标等信息
* Display: 显示模块，列表，控制具体的显示内容

### Control

~~~json
{
    "Duration": 40,
    "Action": {
        "TimeOut": "next"
    },
    "Marker": [{
        "Type": "COM",
        "Data": "01"
    }]
}
~~~

* Duration: 显示时间，-1表示永不过时

* Action: 到时处理方式
* Marker: 列表，通知支持多种打标方式，和[打标信息配置](#打标信息配置)对应

### Display

列表，支持多个显示区块，单个显示区块配置如下

~~~json
{
    "DisplayType": "",
    "Position": {
        "PosMode": "center"
    },
    "Size": {
        "Height": 500,
        "Width": 500
    }
}
~~~

#### DisplayType

* TextDisplay:表示显示信息为文本显示

  * TextData: ***string*** , 用来显示文本数据

  * TextFontFamily: ***string*** , 字体

  * TextFontSize：***int*** , 字号的大小

  * IsBold: ***bool*** ,  表示是否需要加粗

* ImageDisplay: 表示显示的类型是图片
  * ImagePath: ***string*** , 图片位置

* Nothing:表示不显示任何

* GifDisplay:表示显示的为gif动图

#### Position

显示位置

* PosMode：**center** | **fixpos**

  * center: 居中显示

  * fixpos: 固定位置

    "PosX":

    "PosY":

#### Size

显示区块大小控制

* Height: 显示的高度
* Width:  显示的宽度

# 示例/Demo

~~~json
{
    "DialogSize": {
        "Type": "full_screen"
    },
    "MarkerInfo": [{
        "Type": "COM",
        "PortName": "COM3",
        "BaudRate": 115200,
        "DataBits": 8,
        "Parity": "No",
        "StopBits": 1,
        "FlowControl": "No",
        "PreFix": "01 E1 01 00"
    }],
    "Instructions": {
        "Control": {
            "Duration": -1,
            "Action": {
                "Key": {
                    "32": "next"
                }
            }
        },
      "Display": [
        {
          "DisplayType": "TextDisplay",
          "TextData": "欢迎参加本次实验！\n本次实验共五轮， 每轮四十次试验。\n实验准备阶段： 被试坐在椅上放松， 双臂自然下垂\n试验开始： 屏幕中心会出现一个十字， 接下来会出现一张图片（左，右两个方向箭头）\n 请根据箭头指示方向进行相应的运动想象（左，右分别对应左手，右手 ）\n实验过程中请避免眨眼和其他动作， 保持放松。\n （按空格继续）\n（按Esc退出程序）",
          "Position": {
            "PosMode": "center"
          },
          "TextFontFamily": "楷体",
          "TextFontSize": "30",
          "isBold": true
        }
      ]
    },
    "TrailList": {
        "Round": 5,
        "LoopTimes": 40,
        "LoopMode": "random",
        "TrailProc": {
            "StaticBlock": {
                "Fixation": {
                    "Control": {
                        "Duration": 10,
                        "Action": {
                            "TimeOut": "next"
                        }
                    },
                    "Display": [{
                        "DisplayType": "ImageDisplay",
                        "ImagePath": "/image/cross.png",
                        "Position": {
                            "PosMode": "center"
                        },
                        "Size": {
                            "Height": 500,
                            "Width": 500
                        }
                    }]
                },
                "Rest": {
                    "Control": {
                        "Duration": 20,
                        "Action": {
                            "TimeOut": "next"
                        }
                    },
                    "Display": [{
                        "DisplayType": "Nothing"
                    }]
                }
            },
            "StimulusBlock": [{
                "Control": {
                    "Duration": 40,
                    "Action": {
                        "TimeOut": "next"
                    },
                    "Marker": [{
                        "Type": "COM",
                        "Data": "01"
                    }]
                },
                "Display": [{
                    "DisplayType": "ImageDisplay",
                    "ImagePath": "/image/left-arrow.gif",
                    "Position": {
                        "PosMode": "center"
                    },
                    "Size": {
                        "Height": 500,
                        "Width": 500
                    }
                }]
            },{
                "Control": {
                    "Duration": 40,
                    "Action": {
                        "TimeOut": "next"
                    },
                    "Marker": [{
                        "Type": "COM",
                        "Data": "02"
                    }]
                },
                "Display": [{
                    "DisplayType": "ImageDisplay",
                    "ImagePath": "/image/right-arrow.gif",
                    "Position": {
                        "PosMode": "center"
                    },
                    "Size": {
                        "Height": 500,
                        "Width": 500
                    }
                }]
            }],
            "Trail": [{
                "Type": "StaticBlock",
                "Data": "Rest"
            }, {
                "Type": "StaticBlock",
                "Data": "Fixation"
            }, {
                "Type": "StimulusBlock"
            }]
        },
        "BetweenRound": {
            "Control": {
                "Duration": 200,
                "Action": {
                    "TimeOut": "next"
                }
            },
          "Display": [
            {
              "DisplayType": "TextDisplay",
              "TextData": "本轮试验结束，请休息20s后进行下一轮试验",
              "Position": {
                "PosMode": "center"
              },
              "TextFontFamily": "楷体",
              "TextFontSize": "30",
              "isBold": true
            }
          ]
        }
    },
    "GoodBye": {
        "Control": {
            "Duration": -1
        },
      "Display": [
        {
          "DisplayType": "TextDisplay",
          "TextData": "实验结束，感谢您的参与！\n（按Esc退出程序）",
          "Position": {
            "PosMode": "center"
          },
          "TextFontFamily": "楷体",
          "TextFontSize": "30",
          "isBold": true
        }
      ]
    }
~~~
