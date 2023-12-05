# WP Change Daemon

**Brief description**

This is a wallpaper-changing daemon that starts at the beginning of the system, based on feh. You can replace it with your command on your PC.

> For ubuntu
>
> ```shell
> sudo apt-get install feh
> ```

| Date       | Author  | Description   |
| ---------- | ------- | ------------- |
| 2023/12/04 | Manfred | First release |



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
| 3     | 0    | Restart program                        |
| 3     | 1    | Exit program                           |
| 4 - 9 |      | Reserved                               |

**Example**

31 means:

> Event : 3
>
> Code : 1
>
> Action :   Exit program



## Core structure

























