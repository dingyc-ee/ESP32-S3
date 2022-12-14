# ESP32-VSCODE环境搭建  
## 环境安装  
1. 在Windows中安装ESP-IDF
2. 在vscode中安装Espressif IDF插件
3. 开始配置Espressif IDF插件
    在vscode最上方点击：查看->命令面板，输入esp-idf:config，选择ESP-IDF:Configure ESP-IDF extension
    ![配置ESP-IDF插件](./picture/esp32-config.png)  
4. 选择EXPRESS
    ![配置EXPRESS](./picture/esp32-express.png)  
5. Select download server选择乐鑫的Espressif，不要选github，这样比较快，然后install，等待安装完成
    ![下载服务器](./picture/esp32-download-server.png)    

## 新建工程  
#### 方式1：直接复制examples  
1. 从examples中复制demo到文件夹
    ![复制demo工程](./picture/esp32-copy-project.png)  
    再使用vscode打开
    ![vscode1](./picture/esp32-copy-project-vscode1.png)  
2. 在vscode最上方点击：查看->命令面板，输入esp-idf:vscode，选择ESP-IDF:Add vscode configuration folder
    ![vscodd2](./picture/esp32-copy-project-vscode2.png)  
    重新加载使配置生效
3. 设置串口和esp-target
    ![设置串口和芯片类型](./picture/esp32-copy-project-vscode3.png)  
###方式2：使用vscode创建工程
1. 在vscode最上方点击：查看->命令面板，输入esp-idf:new project
    ![使用vscode新建工程](./picture/esp32-vscode-new-project.png)  
2. 设置工程名和路径，选择芯片和串口，创建工程
    ![配置工程](./picture/esp32-vscode-set-project.png)  
3. 选择ESP-IDF，选择工程模板，再创建工程
    ![选择模板](./picture/esp32-vscode-set-project-2.png)  

## 编译工程  
1. Espressif-vscode简介
    ![Expressif配置项](./picture/esp32-vscode-project-introduce.png)  
2. 选择menuconfig，配置flash大小（我的板子是ESP32SC-N8R8，8M Flash）
    ![配置flash大小](./picture/esp32-menuconfig-flashsize.png)  
3. 编译输出，按Ctrl + ] 可以结束打印
    ![配置flash结果](./picture/esp32-menuconfig-flashsize2.png)  
