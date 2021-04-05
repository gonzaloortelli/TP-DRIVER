bash -c "make clean & make"
bash -c "sudo insmod miModulo.ko"
bash -c "dmesg"
read -p "Major number: " major
read -p "Minor number: " minor
bash -c "sudo mknod /dev/SORII c $major $minor"
bash -c "sudo chmod 777 /dev/SORII"
echo "Configuracion exitosa !"