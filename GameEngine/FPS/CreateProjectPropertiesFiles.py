import xml.etree.ElementTree as ET
import xml.dom.minidom
import re
import os

# Set up the MSBuild XML namespace so Visual Studio can read the output
NAMESPACE = 'http://schemas.microsoft.com/developer/msbuild/2003'
ET.register_namespace('', NAMESPACE)
ns = {'ms': NAMESPACE}

def prettify(elem):
    """Formats the XML so it is human-readable with standard indentation."""
    rough_string = ET.tostring(elem, 'utf-8')
    reparsed = xml.dom.minidom.parseString(rough_string)
    # Clean up empty lines introduced by minidom's parser
    pretty_xml = '\n'.join([line for line in reparsed.toprettyxml(indent="  ").split('\n') if line.strip()])
    return pretty_xml

def extract_to_props(vcxproj_path):
    if not os.path.exists(vcxproj_path):
        print(f"Error: Could not find {vcxproj_path}")
        return

    tree = ET.parse(vcxproj_path)
    root = tree.getroot()
    
    # Dictionary to hold our XML elements grouped by their configuration name
    configs = {}

    # Regex to grab 'Debug|x64' from strings like: Condition="'$(Configuration)|$(Platform)'=='Debug|x64'"
    condition_pattern = re.compile(r"==\s*'([^']+)'")

    print(f"Scanning {vcxproj_path} for configuration properties...\n")

    # Look for both setting blocks
    for tag in ['PropertyGroup', 'ItemDefinitionGroup']:
        for elem in root.findall(f'ms:{tag}', ns):
            condition = elem.get('Condition')
            if condition:
                match = condition_pattern.search(condition)
                if match:
                    # Replace the pipe with an underscore for the filename (e.g., 'Debug_x64')
                    config_name = match.group(1).replace('|', '_') 
                    
                    if config_name not in configs:
                        configs[config_name] = []
                    configs[config_name].append(elem)

    if not configs:
        print("No conditional configuration blocks found.")
        return

    # Create a new .props file for each configuration group we found
    for config_name, elements in configs.items():
        
        # FIXED: Let ElementTree handle the xmlns automatically via the URI bracket syntax
        props_root = ET.Element(f'{{{NAMESPACE}}}Project', {'ToolsVersion': '4.0'})
        
        # Add the extracted blocks
        for elem in elements:
            props_root.append(elem)
        
        # Save out the formatted XML
        output_filename = f"{config_name}.props"
        with open(output_filename, 'w', encoding='utf-8') as f:
            # We add the standard XML declaration at the top
            f.write("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
            # We slice off the first line of prettify because we manually added the declaration above
            f.write(prettify(props_root).split('?>\n', 1)[-1])
        
        print(f"Successfully generated: {output_filename}")

# --- Run the Script ---
PROJECT_FILE = 'FPS.vcxproj' 
extract_to_props(PROJECT_FILE)
