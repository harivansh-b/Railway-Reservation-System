import os

# Define the base directory where you want to create folders
base_dir = "D:\Project\Reservation page"

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

print("All folders created successfully.")
