# WP Change Daemon

**Brief description**

This is a wallpaper-changing daemon that starts at the beginning of the system, based on feh. You can replace it with your command on your PC.

> For ubuntu
>
> ```shell
> sudo apt-get install feh
> ```

| Date       | Author  | Description                    |
| ---------- | ------- | ------------------------------ |
| 2023/12/04 | Manfred | First release                  |
| 2023/12/05 | Manfred | Add message queue for response |
| 2023/12/05 | Manfred | Add exit program action        |
| 2023/12/05 | Manfred | Add go back switch method      |



## Rough Design

**Features**

- Automatically change the wallpaper at predetermined intervals
- There are 3 methods to switch wallpapers: sequential, random, and by name



**Require**

- Offer a user-friendly interface interface
- Runs well with scripts



## Input/Output

| Event | Code | Action                                 |
| ----- | ---- | -------------------------------------- |
| 0     | 0    | Disable change-wallpaper automatically |
| 0     | 1    | Enable change-wallpaper automatically  |
| 1     | 0    | Switch **default method** to sequence  |
| 1     | 1    | Switch **default method** to random    |
| 2     | 0    | Set wallpaper sequentially             |
| 2     | 1    | Set wallpaper randomly                 |
| 3     | 0    | Go back to the previous wallpaper      |
| 3     | 1    | Exit program                           |
| 4 - 9 |      | Reserved                               |

**Example**

21 means:

> Event : 2
>
> Code : 1
>
> Action :   Set wallpaper randomly



## Guide

First you need to create a directory to store wallpapers

























