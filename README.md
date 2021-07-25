# Faerie Advanced Locomotion System
![image](https://github.com/Drakynfly/ALSV4_CPP_Faerie/raw/main/Resources/Readme_Content_2.gif)

## About
- This fork is my (Guy Lundvall, aka Drakynfly) customized version of ALSV4_Community by dyanikoglu that adds features that I need for my project, and feel that I should leave here for anyone else who needs it, since I've gotten so much out of this plugin. 

## Features
- All the features of the original. I generally keep this up-to-date with the latest release.
- Flight support. This is the main point of this fork, and hence the name for it: Faerie ALS.
- Swimming support planned. (Currently the framework is added but no animations)
- Integration with other gameplay systems: Movement adjusts to world temperature (e.g, a weather system), and player weight (e.g. an inventory system).
- Extra mantling features: Auto-mantling over short obstacles.
- Easier project integration: there is no need to modify config files in this fork. See #Setting Up The Plugin.

## Known Issues
- No animations for flying and swimming right now, just the framework.
- See [Issues](https://github.com/dyanikoglu/ALSV4_CPP/issues) section

## Setting Up The Plugin
- Clone the repository inside your project's `Plugins` folder, or download the latest release and extract it into your project's or engine's `Plugins` folder.
- Launch the project, and enable plugin content viewer as seen below. This will show contents of the plugin in your content browser:
![image](https://github.com/Drakynfly/ALSV4_CPP_Faerie/raw/main/Resources/Readme_Content_1.png)
- This fork removes the need for any config file changes. There are now settings availible in Project Settings menu. Setup Axis inputs, trace channels, collision profiles, and more.
- This fork seperates the functionality for controlling ALSBaseCharacter into a child class: ALSPlayerCharacter. Parent player controlled characters to this class for access to the rest of the input functions (which are renamed to all begin with "Input_").

## Functions To Override In Blueprints
- There are a few functions you will need to override in blueprints. You will also need to update these functions for every new overlay state you add. These are commented in the C++ code, but are listed here for quick reference as well.
- In your player character blueprint: **GetGetUpAnimation** & **GetRollAnimation**
- In the MantleComponent: **GetMantleAsset**

## License & Contribution
**Source code** of the plugin is licensed under MIT license, and other developers are encouraged to fork the repository, open issues & pull requests to help the development.
