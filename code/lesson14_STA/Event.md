# WIFI事件处理


### 事件描述
<font color='green'>绿色部分表示SDK事件任务默认行为，</font><font color='red'>红色部分表示应用程序通常要执行的动作。</font>


| Event | Description |
|-------|-------------|
| WIFI_EVENT_STA_START | 调用函数 esp_wifi_start() 返回 ESP_OK，产生此事件。<br><font color='green'>事件任务初始化 LwIP 网络接口 (netif)。</font><br><font color='red'>通常，应用程序的事件回调函数需调用 esp_wifi_connect() 来连接已配置的 AP。</font> |
| WIFI_EVENT_STA_STOP | 调用函数 esp_wifi_stop() 到返回 ESP_OK，产生此事件。<br><font color='green'>事件任务将进行释放 IP 地址、终止 DHCP 服务、移除 Socket 并清除 LwIP netif。</font><br><font color='red'>应用程序的事件回调函数通常不需做任何响应。</font> |
| WIFI_EVENT_STA_CONNECTED | 调用函数 esp_wifi_connect() 返回 ESP_OK，且成功连接，产生此事件。<br><font color='green'>事件任务将启动 DHCP 客户端服务并开始获取 IP 地址。此时，Wi-Fi 驱动程序已准备就绪，可发送和接收数据。</font><br><font color='red'>如果您的应用程序不依赖于 LwIP，则此刻便可以开始应用程序开发工作。<br>但是，如果您的应用程序需基于 LwIP 进行，则还需等待 got ip 事件发生后才可开始。</font> |
| WIFI_EVENT_STA_DISCONNECTED | 1. 调用函数 esp_wifi_disconnect()、esp_wifi_stop() 或 esp_wifi_deinit()。<br>2. 调用函数 esp_wifi_connect()，但 Wi-Fi 驱动程序连接失败。<br>3. Wi-Fi 连接因为中断，例如：STA 连续丢失beacon、AP 脱离 station 连接、AP 验证模式改变。<br><font color='green'>事件任务的动作：关闭 STA 的 LwIP netif；通知 LwIP 任务清除所有Socket。</font><br><font color='red'>应用程序通常调用函数 esp_wifi_connect() 重新连接 Wi-Fi。</font> |
| IP_EVENT_STA_GOT_IP | DHCP 客户端成功或 IPV4 地址发生改变时，产生此事件。<br><font color='red'>应用程序一切就绪，可以开始任务（如：创建套接字）。</font> |

