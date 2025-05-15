# 4\.小智使用规范介绍

通过免编译烧录或编译烧录之后，小智的画面含义及蓝牙连接注意事项如下：

## 1. 烧录固件后小智初始界面

<img src="assets/xiaozhi_ready.png" style="width: 50%; display: block; margin: auto;">

log初始化界面，有如下log打印说明固件正确，开发板进入了蓝牙扫描模式，正在等待手机设备连接：
```Bash
<< Write scan enable success
<< Scan mode: 3
<<            Inquiry scan enbd.
<<            Page scan enbd.
```
![](assets/log.png)

---

## 2. 开始蓝牙连接

在连接板子设备之前，请打开手机的蓝牙网络共享功能！！！

以下是 Android 手机的蓝牙设置界面，通过打开个人热点共享中的蓝牙共享网络功能，小智才能共享我们手机的数据网络，实现网络连接。

| ![](assets/Android_ble_2.png) | ![](assets/Android_ble_1.png) | ![](assets/Android_ble_3.png) |
|-------------------------------|-------------------------------|-------------------------------|

IOS 同样需要打开蓝牙共享网络功能，以下是参考步骤（最大兼容性建议关闭，可能影响网络稳定）：

| ![](assets/ios_ble_1.png) | ![](assets/ios_ble_2.png) | ![](assets/ios_ble_3.png) |
|---------------------------|---------------------------|---------------------------|

---

## 3. 激活设备

确保蓝牙共享网络已打开，开始连接蓝牙 `sifli-pan` 设备。

以下是 Android 手机连接状态示例图:
<img src="assets/ble_connect.png" style="width: 50%; display: block; margin: auto;">

---


!!! warning
    一般情况下，Android 连接成功后，连接的蓝牙设备会显示正在向设备共享网络（IOS 不会显示），我们可以以此确定是否成功开启蓝牙网络共享。

连接上 `sifli-pan` 设备后，开发板会有小智连接过程画面提示，出现连接成功画面时此时按下对话按键 (key1)，小智则会提示需要登录到控制面板，填设备码。

| ![](assets/xiaozhi_ready.png) | ![](assets/xiaozhi_pan_connect.png) |
|-------------------------------|------------------------------------|
![](assets/xiaozhi_connect.png) | ![](assets/control.png)

这个时候，打开浏览器，输入网址：[xiaozhi.me](https://xiaozhi.me)。浏览器用手机或者电脑都可以打开。  
进入小智 AI 的网页后，点击控制台，用手机号登录。

![](assets/control_net.png)
![](assets/register.png)
![](assets/new_xiaozhi.png)
![](assets/number.png)

---

## 4. 界面提示含义

出现下方 UI 提示均是 PAN 断开的情况：

| ![](assets/no_pan.png) | ![](assets/pan_disconnect.png){width=60%} | ![](assets/no_pan3.png){width=60%} |
|:----------------------:|:------------------------------------------:|:------------------------------------:|

---

## 5. 异常情况

### 5.1 对应手机的显示可能是：
- 未打开蓝牙共享直接连接 `sifli-pan` 设备
- 蓝牙共享网络关闭
- 蓝牙已断开

### 5.2 解决方式：
打开蓝牙共享网络重新连接设备

!!! example
    下图为蓝牙连接成功但未开启蓝牙共享网络的异常示例：

![](assets/no_pan2.png){width=60%}

---

## 6. 唤醒 & 重连

长时间未对话小智会进入休眠，此时需要按下 `key2` 进行唤醒：

![](assets/sleep.png)

支持重连操作：若无主动删除手机匹配列表下的 `sifili-pan` 设备，远离设备导致的断开的情况，按下 `key1` 也可进行蓝牙重连：

| ![](assets/pan_rec.png) | ![](assets/pan_rec_sucf.png) |
|:-----------------------:|:----------------------------:|