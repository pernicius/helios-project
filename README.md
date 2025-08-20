# The Helios Project (C++)
Dieses Projekt ist eine längerfristige Beschäftigung. Endziel ist eine möglichst performant 3D-Engine und einige Tools um deren Assets zu Verwalten/Erstellen.

***

## Getting Started
Ich persönlich favorisiere Visual Studio, andere Entwicklungsumgebungen sind offiziel ungetestet. Auch wenn das Projekt andere Betriebssysteme unterstützen soll liegt der primäre Focus momentan bei Windows.

<ins>**1. Download:**</ins>

Start by cloning the repository with:<br />
`git clone https://github.com/pernicius/helios-project.git`.

<ins>**1. Configure the dependencies:**</ins>

- Windows / Visual Studio

    1. Start the 'scripts/windows/setup.bat' batch file. Which will then download and install all the required prerequisites for the projekt if they are not present yet.
        - **Premake:** Will be downloaded. [Premake] is the main workspace generator for this project.
        - **Vulkan SDK:** If not present the script will download it and execute the installer.
        - **Git Submodules:** all submodules will be pulled.
        - Start of the [generate.bat] is done automatically.
    2. Start the 'scripts/windows/generate.bat' batch file. Which will then generate a Visual Studio solution file.
    
	If changes are made, or if you want to regenerate the project files, rerun the 'scripts/windows/generate.bat' batch file.

***
