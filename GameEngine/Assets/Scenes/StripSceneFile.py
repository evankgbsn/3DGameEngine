import xml.etree.ElementTree as ET

def convert_to_instanced(input_filepath, output_filepath):
    # Parse the XML file
    tree = ET.parse(input_filepath)
    root = tree.getroot()
    
    # Find all Shipping Containers
    containers = root.findall(".//Object[@type='ShippingContainer']")
    if not containers:
        print("No shipping containers found.")
        return

    transforms = []

    # 1. Process every container to extract transforms and strip old graphics
    for i, container in enumerate(containers):
        # Find the old Graphics component
        graphics_comp = container.find(".//Component[@name='Graphics']")
        
        if graphics_comp is not None:
            # Extract the Transform matrix
            transform_var = graphics_comp.find(".//Variable[@name='Transform']")
            if transform_var is not None:
                # Rename the variable name to match its new array index (0, 1, 2, etc.)
                transform_var.set("name", str(i))
                transforms.append(transform_var)
            
            # Remove the old graphics component
            container.remove(graphics_comp)
        
        # Add the 'Empty' component with the InstanceID to tie physics back to graphics
        empty_comp = ET.Element("Component", type="", name="Empty")
        var_inst = ET.SubElement(empty_comp, "Variable", type="int", name="InstanceID")
        ET.SubElement(var_inst, "int", val=str(i))
        
        # Insert it at the top of the container's component list
        container.insert(0, empty_comp)

    # 2. Build the new Instanced Graphics Component on the FIRST container
    master_container = containers[0]
    instanced_graphics = ET.Element("Component", type="GraphicsObjectTexturedLitInstanced", name="Graphics")
    
    # Add Shine
    shine_var = ET.SubElement(instanced_graphics, "Variable", type="float", name="Shine")
    ET.SubElement(shine_var, "float", val="4.000000")
    
    # Add InstanceCount
    count_var = ET.SubElement(instanced_graphics, "Variable", type="int", name="InstanceCount")
    ET.SubElement(count_var, "int", val=str(len(transforms)))
    
    # Append all the extracted transform matrices
    for t in transforms:
        instanced_graphics.append(t)
        
    # Add Texture & Model Data
    model_var = ET.SubElement(instanced_graphics, "Variable", type="string", name="ModelName")
    ET.SubElement(model_var, "string", val="ShippingContainer")

    diffuse_var = ET.SubElement(instanced_graphics, "Variable", type="string", name="DiffuseTextureName")
    ET.SubElement(diffuse_var, "string", val="ShippingContainerRed")

    spec_var = ET.SubElement(instanced_graphics, "Variable", type="string", name="SpecularTextureName")
    ET.SubElement(spec_var, "string", val="ShippingContainerSpec")

    normal_var = ET.SubElement(instanced_graphics, "Variable", type="string", name="NormalTextureName")
    ET.SubElement(normal_var, "string", val="ShippingContainerNormal")

    # Attach this master component to the first container
    master_container.append(instanced_graphics)

    # 3. Save the new XML
    tree.write(output_filepath, encoding="utf-8", xml_declaration=True)
    print(f"Success! Converted {len(transforms)} containers to instanced rendering.")
    print(f"Saved to: {output_filepath}")

if __name__ == "__main__":
    convert_to_instanced("FPS.xml", "FPS_Instanced_Converted.xml")
