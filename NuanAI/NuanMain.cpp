#include <iostream>
#include <Windows.h>
#include <stdio.h>  
#include <string.h>
#include <tchar.h>
#include "obj.h"
#include "NuanMain.h"
#include"NGame.h"
// main.cpp



using namespace std;


int main()
{


      
            NGame game;

             //初始化游戏，传入游戏窗口名称
            if (!game.Initialize("无限暖暖")) {
                return -1; // 如果初始化失败，则退出程序
            }
            
            game.fangiangtest();
 

    return 0;

        // 设置键盘钩子
        //HookKeyboard();
    }
