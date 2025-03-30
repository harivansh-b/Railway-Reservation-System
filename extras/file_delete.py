import os

# Define the base directory where you want to delete the file
base_dir = "D:\Project\Reservation page"

# File name to delete from each folder
file_to_delete = "Kovai SF Express-12675.csv"

# Iterate over the folders
for i in range(1, 32):
    folder_name = str(i)
    folder_path = os.path.join(base_dir, folder_name)
    
    # Check if the file exists in the folder
    file_path = os.path.join(folder_path, file_to_delete)
    if os.path.exists(file_path):
        # Delete the file
        os.remove(file_path)
        print(f"File '{file_to_delete}' deleted from folder '{folder_name}'.")
    else:
        print(f"File '{file_to_delete}' not found in folder '{folder_name}'.")

print("All files deleted successfully.")
