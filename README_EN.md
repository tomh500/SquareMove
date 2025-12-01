# FreeMove_Square


## Project name aliases

FreeMoveSQ  
FreeMove-SQ  
FreeMove_SQ  
FreeMove Square  
FreeMoveSquare 
FreeMove

---
[中文说明](README.md)

FreeMove_Square is a Windows utility that allows you to **move files and directories without breaking programs, shortcuts, or system paths**.

It safely moves content and automatically creates system links so that software continues to work as if nothing changed.

---

## Features

FreeMove_Square allows you to:

- Move files and directories between disks
- Automatically create symbolic links (mklink)
- Fast Mode (same-disk move)
- Copy Mode with safe cleanup
- Option to hide original path
- Multi-language UI (EN / CN)
- Drag & drop support
- Administrator privilege detection

---

## How It Works

1. Files are moved to a new location
2. A symbolic link is created at the original path
3. Programs automatically access the new location
4. The old path keeps working transparently

---

## Usage

1. Run the program as **Administrator**
2. Select:
   - Source file or folder
   - Destination directory
3. Click “Move”
4. Done — the original path is still accessible

---

## Important Warning

⚠ **Do NOT move system directories**

Avoid moving:

- `C:\Windows`
- `C:\Program Files`
- `C:\Program Files (x86)`
- `C:\Users`
- `C:\ProgramData`

You may safely move subfolders inside them, such as:
```
C:\Users\Name\AppData\Local\Cache
```

---

## Uninstalling moved programs

Uninstall moved software normally.  
After uninstall:

- You may delete the symbolic link manually
- Clean up empty folders if needed

---

## Restore original location

1. Delete the symbolic link
2. Move the folder back to its original path

---

## Administrator Privileges

Administrator rights are required to:

- Create symbolic links
- Access protected folders
- Modify file attributes

If run without admin rights, the program will prompt for elevation.

---


## License

This project is licensed under GPL v3.

---

