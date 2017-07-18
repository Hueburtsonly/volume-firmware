## Synopsis

This glorious piece of firmware is responsible for controlling the world's **BEST** mixer interface hardware.

## Setup 

This software is built using NXP LPCXpresso. This tool is free but requires a registration and a multi-step activation.

* Install your venti-soy-quadruple-shot-trim LPCXpresso IDE. [Latest from here possibly.](https://nxp.flexnetoperations.com/control/frse/product?entitlementId=240358107&lineNum=1)

* Create a serial number: Help -> Activate -> Create serial number and register (Free edition)...

* The IDE will open an NXP webpage. Log in to NXP website and accept the EULA, which will generate an activation key (which should also be emailed to you).

* Enter activation code: Help -> Activate -> Activate (Free edition)...

* Add EGit for LPCTrimLatte. [Instructions here (possibly).](http://download.eclipse.org/egit/updates/)

* Import project into workspace: File -> Import -> Git -> Projects from Git -> (clone or local)

* Add required dependency _lpc_chip_11uxx_lib_ from _LPCOpen/lpcopen_v2_03_lpcxpresso_nxp_lpcxpresso_11u37h.zip_. This _should_ live in an **Examples** subdirectory of the install directory. Default path (possibly): **C:\nxp\LPCXpresso_8.2.2_650\lpcxpresso\Examples\LPCOpen\lpcopen_v2_00a_lpcxpresso_nxp_lpcxpresso_11u14.zip**. File -> Import -> General -> Existing Projects Into Workspace -> Select archive file -> (Select lpc_chip_11uxx_lib) -> Finish

* Compile! Project -> Build All (followed by a quiet but fervent prayer).
