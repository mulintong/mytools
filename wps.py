import os
import os.path
import stat
import sys
import winreg

global wps_install_path

def find_wps_install_path():
    key = winreg.OpenKey(winreg.HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Kingsoft Office")
    install_path = winreg.QueryValueEx(key, "LocationRoot")[0]
    return install_path

def assemble_file(f):
    global wps_install_path
    full_path = os.path.join(wps_install_path, f)
    return full_path

def truncate_file(file_path):
    try:
        print("Begin truncate file \"%s\"" % (file_path))

        if not os.path.exists(file_path) or (os.path.getsize(file_path) == 0):
            print("\"%s\" not exist or is empty, skip it..." % file_path)
            return True

        with open(file_path, "w") as f:
            f.close()
            os.chmod(file_path, stat.S_IREAD)
            print("Truncate finish...")
            return True
    except PermissionError as e:
        print("Truncate exception: ", e)
    
    print("Truncate failed!")
    return False

def delete_wps_disk():
    try:
        winreg.DeleteKey(winreg.HKEY_CURRENT_USER,
            "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MyComputer\\NameSpace\\{5FCD4425-CA3A-48F4-A57C-B8A75C32ACB1}")
    except:
        print("WPS disk may not exist")

    print("Delete wps disk finish...")


if __name__ == "__main__":
    try:
        # For example C:\Program Files (x86)\WPS Office\11.1.0.7764
        global wps_install_path
        wps_install_path = find_wps_install_path()

        if os.path.exists(wps_install_path):
            print("WPS Office is installed in \"%s\"" % (wps_install_path))
        else:
            print("WPS Office not found...")
            sys.exit(0)

        # Kill related processes
        os.system("taskkill /F /IM wps.exe")
        os.system("taskkill /F /IM wpscenter.exe")
        os.system("taskkill /F /IM wpsoffice.exe")
        os.system("taskkill /F /IM explorer.exe") # qingbangong.dll loaded in explorer.exe

        delete_wps_disk()

        # Truncate file and make it readonly
        junk_files = [
            "office6\\addons\\qing\\qingbangong.dll",
            "office6\\wpscenter.exe",
            "office6\\wpscloudlaunch.exe",
            "office6\\wpscloudsvr.exe",
            "office6\\wpscloudsvrimp.dll",
            "wtoolex\\desktoptip.exe",
            "wtoolex\\updateself.exe",
            "wtoolex\\wpsnotify.exe",
            "wtoolex\\wpsupdate.exe"]
        junk_files = list(map(assemble_file, junk_files))

        # For example C:\Program Files (x86)\WPS Office
        parent_dir = os.path.split(wps_install_path)[0]
        junk_files.insert(0, os.path.join(parent_dir, "wpscloudsvr.exe"))

        for junkf in junk_files:
            truncate_file(junkf)

        print("Clean finish...")
    except:
        print("Encountered an exception")
        sys.exit(0)
    finally:
        print("Restart explorer.exe...")
        os.system("start explorer.exe...")

    print("Exit...")
    
