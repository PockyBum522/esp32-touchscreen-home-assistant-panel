# pc_simulator_win_codeblocks
PC simulator project for LittlevGL embedded GUI Library for Windows in Codeblocks

This requires the MinGW version of **Code::blocks 20.03 or newer** to be installed.

## How to use this:

###  Tutorial for Windows
1) Download and install [Git](https://git-scm.com/downloads) 
2) Download [CodeBlocks](https://www.codeblocks.org/downloads/) in latest version. It is *strongly* recommended to use the version which includes MinGW, as otherwise you will have to install and configure it separately, and there have been issues reported with missing header files when a standalone MinGW installation is used.
3) If the version of Code::blocks <= 20.03, then MinGW need [upgrade](docs/upgrade_MinGW.md).
4) Clone this repository:
    - Open **Command prompt (Win key + R -> cmd -> Enter)** or **PowerShell (Win key + R -> powershell -> Enter)** or **Git bash** (if you want different folder than _C:/Users/username_ you have to navigate to it) and type: `git clone --recurse-submodules https://github.com/lvgl/lv_port_win_codeblocks.git`
5) Open **CodeBlocks** and first [set use flat objcts](docs/set_use_flat_objects.md).
6) And then select **Open an existing project**. Navigate to the **lv_sim_codeblocks_win** folder and select **lvgl.cbp**.
7) Click on **Build and Run** or press **F9**.
8) If everything goes well, you should see your simulator running.

