# D.A.U.S.E.  
Digital Amplified User System Environment  

## Dependencies  
### Qt 6
`sudo pacman -S qt6-base qt6-tools`

### QTermWidget (https://github.com/lxqt/qtermwidget/)
`sudo pamac build qtermwidget-git`    

### QMediaPlayer
`sudo pacman -S qt6-multimedia` → Choose FFmpeg Option (**1**)  

### FFTW (https://www.fftw.org/)  
`sudo pacman -S fftw`  

### FFmpeg  
`sudo pacman -S ffmpeg`  

## To-Fix  
Duplicated Powerlines when spawning a new terminal (sometimes)  

## Notes  
If Delete key (from physical keyboard) removes words and not characters, add the following lines at the bottom of the **~/.zshrc** file:  
`bindkey '^?' backward-delete-char`  
`bindkey '^H' backward-delete-char`  