import os
import shutil

# Define the base directory where you want to create folders
base_dir = "D:\Project\Reservation page"

# Path to the file you want to copy
file_to_copy = r"Reservation page\Vaigai Express-12635.csv"

# Create folders
for i in range(1, 32):
    folder_name = str(i)
    folder_path = os.path.join(base_dir, folder_name)
    
    # Check if folder already exists, if not, create it
    if not os.path.exists(folder_path):
        os.makedirs(folder_path)
        print(f"Folder '{folder_name}' created successfully.")
    else:
        print(f"Folder '{folder_name}' already exists.")
    
    # Copy the file into the folder
    shutil.copy(file_to_copy, folder_path)
    print(f"File '{os.path.basename(file_to_copy)}' copied to folder '{folder_name}'.")

print("All folders created and files copied successfully.")
