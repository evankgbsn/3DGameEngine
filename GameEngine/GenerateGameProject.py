import os
import uuid
import argparse

def generate_project_files(project_name, output_dir="."):
    # Generate the fresh GUID
    new_guid = f"{{{str(uuid.uuid4()).upper()}}}"
    
    # 1. Main .vcxproj Template (Fully populated with 16 Configs AND Engine Reference)
    vcxproj_template = """<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
	  <ProjectConfiguration Include="DebugClientEditor|Win32">
		  <Configuration>DebugClientEditor</Configuration>
		  <Platform>Win32</Platform>
	  </ProjectConfiguration>
	  <ProjectConfiguration Include="DebugClientEditor|x64">
		  <Configuration>DebugClientEditor</Configuration>
		  <Platform>x64</Platform>
	  </ProjectConfiguration>
	  <ProjectConfiguration Include="DebugClientGame|Win32">
		  <Configuration>DebugClientGame</Configuration>
		  <Platform>Win32</Platform>
	  </ProjectConfiguration>
	  <ProjectConfiguration Include="DebugClientGame|x64">
		  <Configuration>DebugClientGame</Configuration>
		  <Platform>x64</Platform>
	  </ProjectConfiguration>
	  <ProjectConfiguration Include="DebugServerEditor|Win32">
		  <Configuration>DebugServerEditor</Configuration>
		  <Platform>Win32</Platform>
	  </ProjectConfiguration>
	  <ProjectConfiguration Include="DebugServerEditor|x64">
		  <Configuration>DebugServerEditor</Configuration>
		  <Platform>x64</Platform>
	  </ProjectConfiguration>
	  <ProjectConfiguration Include="DebugServerGame|Win32">
		  <Configuration>DebugServerGame</Configuration>
		  <Platform>Win32</Platform>
	  </ProjectConfiguration>
	  <ProjectConfiguration Include="DebugServerGame|x64">
		  <Configuration>DebugServerGame</Configuration>
		  <Platform>x64</Platform>
	  </ProjectConfiguration>
	  <ProjectConfiguration Include="ReleaseClientEditor|Win32">
		  <Configuration>ReleaseClientEditor</Configuration>
		  <Platform>Win32</Platform>
	  </ProjectConfiguration>
	  <ProjectConfiguration Include="ReleaseClientEditor|x64">
		  <Configuration>ReleaseClientEditor</Configuration>
		  <Platform>x64</Platform>
	  </ProjectConfiguration>
	  <ProjectConfiguration Include="ReleaseClientGame|Win32">
		  <Configuration>ReleaseClientGame</Configuration>
		  <Platform>Win32</Platform>
	  </ProjectConfiguration>
	  <ProjectConfiguration Include="ReleaseClientGame|x64">
		  <Configuration>ReleaseClientGame</Configuration>
		  <Platform>x64</Platform>
	  </ProjectConfiguration>
	  <ProjectConfiguration Include="ReleaseServerEditor|Win32">
		  <Configuration>ReleaseServerEditor</Configuration>
		  <Platform>Win32</Platform>
	  </ProjectConfiguration>
	  <ProjectConfiguration Include="ReleaseServerEditor|x64">
		  <Configuration>ReleaseServerEditor</Configuration>
		  <Platform>x64</Platform>
	  </ProjectConfiguration>
	  <ProjectConfiguration Include="ReleaseServerGame|Win32">
		  <Configuration>ReleaseServerGame</Configuration>
		  <Platform>Win32</Platform>
	  </ProjectConfiguration>
	  <ProjectConfiguration Include="ReleaseServerGame|x64">
		  <Configuration>ReleaseServerGame</Configuration>
		  <Platform>x64</Platform>
	  </ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <VCProjectVersion>18.0</VCProjectVersion>
    <Keyword>Win32Proj</Keyword>
    <ProjectGuid>%%PROJECT_GUID%%</ProjectGuid>
    <RootNamespace>%%PROJECT_NAME%%</RootNamespace>
    <WindowsTargetPlatformVersion>$(LatestTargetPlatformVersion)</WindowsTargetPlatformVersion>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\\Microsoft.Cpp.Default.props" />
  <PropertyGroup>
    <TargetName>$(ProjectName)$(Configuration)</TargetName>
  </PropertyGroup>
  
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='DebugClientEditor|Win32'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>$(DefaultPlatformToolset)</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='DebugClientEditor|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>$(DefaultPlatformToolset)</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='DebugClientGame|Win32'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>$(DefaultPlatformToolset)</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='DebugClientGame|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>$(DefaultPlatformToolset)</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='DebugServerEditor|Win32'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>$(DefaultPlatformToolset)</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='DebugServerEditor|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>$(DefaultPlatformToolset)</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='DebugServerGame|Win32'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>$(DefaultPlatformToolset)</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='DebugServerGame|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>$(DefaultPlatformToolset)</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>

  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='ReleaseClientEditor|Win32'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>$(DefaultPlatformToolset)</PlatformToolset>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='ReleaseClientEditor|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>$(DefaultPlatformToolset)</PlatformToolset>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='ReleaseClientGame|Win32'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>$(DefaultPlatformToolset)</PlatformToolset>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='ReleaseClientGame|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>$(DefaultPlatformToolset)</PlatformToolset>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='ReleaseServerEditor|Win32'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>$(DefaultPlatformToolset)</PlatformToolset>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='ReleaseServerEditor|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>$(DefaultPlatformToolset)</PlatformToolset>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='ReleaseServerGame|Win32'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>$(DefaultPlatformToolset)</PlatformToolset>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='ReleaseServerGame|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>$(DefaultPlatformToolset)</PlatformToolset>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>

  <Import Project="$(VCTargetsPath)\\Microsoft.Cpp.props" />
  <ImportGroup Label="ExtensionSettings">
  </ImportGroup>
  <ImportGroup Label="Shared" >
  </ImportGroup>
  
  <PropertyGroup Label="UserMacros" />

  <ItemGroup>
    <ProjectReference Include="..\\GameEngine\\GameEngine.vcxproj">
      <Project>{48ae9f95-f35d-44d7-9aad-1ae66c659832}</Project>
    </ProjectReference>
  </ItemGroup>

  <Import Project="$(VCTargetsPath)\\Microsoft.Cpp.targets" />
  <ImportGroup Label="ExtensionTargets">
  </ImportGroup>
  
	<ImportGroup Label="PropertySheets">
		<Import Project="$(SolutionDir)\\GameProjectProperties\\DebugClientEditor_Win32.props" Condition="'$(Configuration)|$(Platform)'=='DebugClientEditor|Win32'" />
		<Import Project="$(SolutionDir)\\GameProjectProperties\\DebugClientEditor_x64.props" Condition="'$(Configuration)|$(Platform)'=='DebugClientEditor|x64'" />

		<Import Project="$(SolutionDir)\\GameProjectProperties\\DebugClientGame_Win32.props" Condition="'$(Configuration)|$(Platform)'=='DebugClientGame|Win32'" />
		<Import Project="$(SolutionDir)\\GameProjectProperties\\DebugClientGame_x64.props" Condition="'$(Configuration)|$(Platform)'=='DebugClientGame|x64'" />

		<Import Project="$(SolutionDir)\\GameProjectProperties\\DebugServerEditor_Win32.props" Condition="'$(Configuration)|$(Platform)'=='DebugServerEditor|Win32'" />
		<Import Project="$(SolutionDir)\\GameProjectProperties\\DebugServerEditor_x64.props" Condition="'$(Configuration)|$(Platform)'=='DebugServerEditor|x64'" />

		<Import Project="$(SolutionDir)\\GameProjectProperties\\DebugServerGame_Win32.props" Condition="'$(Configuration)|$(Platform)'=='DebugServerGame|Win32'" />
		<Import Project="$(SolutionDir)\\GameProjectProperties\\DebugServerGame_x64.props" Condition="'$(Configuration)|$(Platform)'=='DebugServerGame|x64'" />

		<Import Project="$(SolutionDir)\\GameProjectProperties\\ReleaseClientEditor_Win32.props" Condition="'$(Configuration)|$(Platform)'=='ReleaseClientEditor|Win32'" />
		<Import Project="$(SolutionDir)\\GameProjectProperties\\ReleaseClientEditor_x64.props" Condition="'$(Configuration)|$(Platform)'=='ReleaseClientEditor|x64'" />

		<Import Project="$(SolutionDir)\\GameProjectProperties\\ReleaseClientGame_Win32.props" Condition="'$(Configuration)|$(Platform)'=='ReleaseClientGame|Win32'" />
		<Import Project="$(SolutionDir)\\GameProjectProperties\\ReleaseClientGame_x64.props" Condition="'$(Configuration)|$(Platform)'=='ReleaseClientGame|x64'" />

		<Import Project="$(SolutionDir)\\GameProjectProperties\\ReleaseServerEditor_Win32.props" Condition="'$(Configuration)|$(Platform)'=='ReleaseServerEditor|Win32'" />
		<Import Project="$(SolutionDir)\\GameProjectProperties\\ReleaseServerEditor_x64.props" Condition="'$(Configuration)|$(Platform)'=='ReleaseServerEditor|x64'" />

		<Import Project="$(SolutionDir)\\GameProjectProperties\\ReleaseServerGame_Win32.props" Condition="'$(Configuration)|$(Platform)'=='ReleaseServerGame|Win32'" />
		<Import Project="$(SolutionDir)\\GameProjectProperties\\ReleaseServerGame_x64.props" Condition="'$(Configuration)|$(Platform)'=='ReleaseServerGame|x64'" />
	</ImportGroup>
	<PropertyGroup>
    <LocalDebuggerWorkingDirectory>$(OutDir)</LocalDebuggerWorkingDirectory>
    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
  </PropertyGroup>
</Project>
"""

    # 2. Filters Template
    filters_template = """<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup>
    <Filter Include="Source Files">
      <UniqueIdentifier>{4FC737F1-C7A5-4376-A066-2A32D752A2FF}</UniqueIdentifier>
      <Extensions>cpp;c;cc;cxx;c++;cppm;ixx;def;odl;idl;hpj;bat;asm;asmx</Extensions>
    </Filter>
    <Filter Include="Header Files">
      <UniqueIdentifier>{93995380-89BD-4b04-88EB-625FBE52EBFB}</UniqueIdentifier>
      <Extensions>h;hh;hpp;hxx;h++;hm;inl;inc;ipp;xsd</Extensions>
    </Filter>
    <Filter Include="Resource Files">
      <UniqueIdentifier>{67DA6AB6-F800-4c08-8B7A-83BB121AAD01}</UniqueIdentifier>
      <Extensions>rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe;resx;tiff;tif;png;wav;mfcribbon-ms</Extensions>
    </Filter>
  </ItemGroup>
</Project>
"""

    # 3. User Template
    user_template = """<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="Current" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <PropertyGroup />
</Project>
"""

    # --- INJECT VARIABLES ---
    vcxproj_final = vcxproj_template.replace("%%PROJECT_GUID%%", new_guid).replace("%%PROJECT_NAME%%", project_name)

    # --- FILE WRITING ---
    try:
        project_dir = os.path.join(output_dir, project_name)

        if not os.path.exists(project_dir):
            os.makedirs(project_dir)

        vcxproj_path = os.path.join(project_dir, f"{project_name}.vcxproj")
        filters_path = os.path.join(project_dir, f"{project_name}.vcxproj.filters")
        user_path = os.path.join(project_dir, f"{project_name}.vcxproj.user")

        with open(vcxproj_path, "w", encoding="utf-8") as f:
            f.write(vcxproj_final)

        with open(filters_path, "w", encoding="utf-8") as f:
            f.write(filters_template)

        with open(user_path, "w", encoding="utf-8") as f:
            f.write(user_template)

        print(f"SUCCESS: Created project stack in directory: {project_dir}")

    except Exception as e:
        print(f"CRITICAL ERROR while trying to write files: {e}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate a new C++ project file stack with engine configurations.")
    parser.add_argument("project_name", help="The name of the new project (e.g., PhysicsCore)")
    parser.add_argument("--out", default=".", help="Output directory (defaults to current folder)")
    
    try:
        args = parser.parse_args()
        generate_project_files(args.project_name, args.out)
    except SystemExit:
        print("\nERROR: You must provide a project name when running this script.")
        print("Usage from terminal: python GenerateGameProject.py MyNewProjectName")
        input("\nPress Enter to exit...")
