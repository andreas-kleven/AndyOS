ip link add name br0 type bridge
ip link set br0 up
ip link set enp0s31f6 up
ip link set enp0s31f6 master br0
bridge link
