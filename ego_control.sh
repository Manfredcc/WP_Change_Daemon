#! /bin/bash

# Check if the 'ego_wp_change_daemon' is already running
_thisdir=$(cd $(dirname $0);pwd)

pgrep -f ego_wp_change_daemon

if [ $? -eq 1 ]; then
    echo "ego_wp_change_daemon is not running, start it"
    select_wp="/home/ll/02_resource/photo/wallpapers/select/"
    ls $select_wp > $_thisdir/wp.txt
    $_thisdir/ego_wp_change_daemon wp.txt &
    echo "cc1"
    exit 1
    echo "cc2"
fi
echo "cc3"

# Send message to daemon by message when it is running

rofi_menu_item() {
    echo 'Set wp sequentially'
    echo 'Go back to previous wp'
    echo 'Set wp randomly'
    echo 'Disable ch-wp auto'
    echo 'Enable ch-wp auto'
    echo 'Set ch-wp auto method to sequence'
    echo 'Set ch-wp auto method to random'
    echo 'Exit program'
}

control() {
    case $1 in
        'Set wp sequentially')
            $_thisdir/send 20
            ;;
        'Go back to previous wp')
            $_thisdir/send 30
            ;;
        'Set wp randomly')
            $_thisdir/send 21
            ;;
        'Disable ch-wp auto')
            $_thisdir/send 00
            ;;
        'Enable ch-wp auto')
            $_thisdir/send 01
            ;;
        'Set ch-wp auto method to sequence')
            $_thisdir/send 10
            ;;
        'Set ch-wp auto method to random')
            $_thisdir/send 11
            ;;
        'Exit program')
            $_thisdir/send 31
            ;;
    esac
}

control "$(rofi_menu_item | rofi -dmenu -p "Ego WP")"
