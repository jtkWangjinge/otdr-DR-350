#!/bin/sh

project=$1
firmware_xxx_md5="firmware_"$project".md5"
firmware_xxx_md5_tar_gz="firmware_"$project"_md5.tar.gz"
firmware_xxx_first_md5="firmware_"$project"_first.md5"
firmware_xxx_first_md5_tar_gz="firmware_"$project"_first_md5.tar.gz"

skin="../Skin.ini"
echo 删除之前的升级包
rm -rf firmware.* -rf
#编译算法源文件，生成库文件
cd ../run/mini1/include
make clean
make -j32
cp libotdr.a ../lib
cd ../../../upgrade
#编译治具工程
# cd ..
# cd fct/
# make clean
# make -j32
# cd ../upgrade
#编译源工程，出错则返回1
cd ../
#make MINI1=1 SDCARD=0 USER_LOG=0 LEVEL_LOG=4  -j32
make -j32 LEVEL_LOG=4 G=0 SDCARD=0 TOUCH_SCREEN=0 OPTIC_DEVICE=1 FIP_DEVICE=1 POWER_DEVICE=1 EEPROM_DATA=1 SKIN_320_240=0 SKIN_640_480=1

if [ $? -eq 0 ];then
    echo 0
else 
    echo "make error..."
	exit 1    
fi
cd ./upgrade/

#为了不copy ".svn"目录
rsync --exclude=".svn" -r --force ../fct ./updatefile/userfs/
rsync --exclude=".svn" -r --force ../bitmap ./updatefile/userfs/
rsync --exclude=".svn" -r --force ../font ./updatefile/userfs/
rsync --exclude=".svn" -r --force ../wndproc ./updatefile/userfs/
rsync --exclude=".svn" -r --force ./app/drivers ./updatefile/userfs/
rsync --exclude=".svn" -r --force ./app/bin ./updatefile/userfs/
cd ./app
tar --exclude=".svn" -czvf lib.tar.gz ./lib
tar -xzvf lib.tar.gz -C ../updatefile/userfs/
rm lib.tar.gz
cd ..

cp ./app/otdrStart.sh ./updatefile/userfs/

cp -rf ./version.bin ./updatefile/userfs/
if [ -f $skin ] ; then
    cp $skin ./updatefile/userfs/ -rf
else
    echo "skin is not exists!"
fi 
chmod +x ./updatefile/userfs/fct/wnd*
chmod +x ./updatefile/userfs/wndproc
chmod +x ./updatefile/userfs/lcd_unblank
chmod +x ./updatefile/userfs/otdrStart.sh

#为了不把".svn"打包进去
#解压full_rootfs
tar zxvf full_rootfs.tar.gz -C ./
sync
cd ./full_rootfs/
tar --exclude=".svn" -czf Rootfs.img *
mv Rootfs.img ../updatefile/images/
cd ../

#
cd ./updatefile/userfs
tar --exclude=".svn" -czf Userfs.img *
mv Userfs.img ../images/
cd ../../

#
cp -rf ./version.bin ./updatefile/images/
cp -rf ./BootStrap.img ./updatefile/images/

echo "make every updatefile"
cd ./updatefile/images/
rm -rf firmware.* -rf
cp -rf ./update_wndproc_every ./update_wndproc
chmod +x ./update_wndproc
tar --exclude=".svn" -czf firmware.tar.gz *
md5sum firmware.tar.gz update_wndproc > $firmware_xxx_md5
tar -czf $firmware_xxx_md5_tar_gz firmware.tar.gz update_wndproc $firmware_xxx_md5
rm -rf ./Rootfs.img
rm -rf ./firmware.tar.gz
rm -rf ./$firmware_xxx_md5
mv ./$firmware_xxx_md5_tar_gz ../../
cd ../../

#解压full_rootfs
#tar zxvf full_rootfs.tar.gz -C ./
#sync
cd ./full_rootfs
tar --exclude=".svn" -czf Rootfs.img *
mv Rootfs.img ../updatefile/images/
cd ../

echo "make first updatefile"
cd ./updatefile/images
cp -rf ./update_wndproc_first ./update_wndproc
chmod +x ./update_wndproc
rm -rf firmware.tar.gz
tar --exclude=".svn" -czf firmware.tar.gz *
md5sum firmware.tar.gz update_wndproc > $firmware_xxx_first_md5
tar -czf $firmware_xxx_first_md5_tar_gz firmware.tar.gz update_wndproc $firmware_xxx_first_md5
mv ./$firmware_xxx_first_md5_tar_gz ../../
cd ../../
