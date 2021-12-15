echo 解压升级压缩包
project=$1
#升级脚本生成的压缩包
old_upgrade_tar_gz="firmware_O10_md5.tar.gz"
old_first_upgrade_tar_gz="firmware_O10_first_md5.tar.gz"
#打包升级文件的文件夹
upgrade_folder="$project"
first_upgrade_folder=""$project"(出场烧录)"
#打包升级文本的文件名
first_upgrade_tar_gz=""$project"(出场烧录).tar.gz"
upgrade_tar_gz=""$project".tar.gz"

tar zxvf upgrade.tar.gz -C ./
cp version.bin upgrade/
#进入upgrade
cd upgrade/
#执行升级程序
chmod +x makeupdatefile.sh
./makeupdatefile.sh O10
#制作升级包
mkdir $upgrade_folder
tar zxvf $old_upgrade_tar_gz -C $upgrade_folder
tar zcvf $upgrade_tar_gz $upgrade_folder
#制作出场烧录包
cp ../first_firmware.tar.gz ./
mkdir $first_upgrade_folder
tar zxvf first_firmware.tar.gz -C $first_upgrade_folder
cd $first_upgrade_folder
mv first_firmware/* ./
rm first_firmware/ -rf
cd ..
tar zxvf $old_first_upgrade_tar_gz -C $first_upgrade_folder
tar zcvf $first_upgrade_tar_gz $first_upgrade_folder
#删除原始文件
rm first_firmware.tar.gz
rm $upgrade_folder -rf
rm $first_upgrade_folder -rf
