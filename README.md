# D.A.U.S.E.   
**Digital Amplified User System Environment** is a retro-futuristic, minimalist terminal emulator purpose-built for **Arch Linux**. It blends the raw aesthetic of vintage computing with high-performance modern features, including integrated media visualization and a robust parallel tab system.  

*DAUSE is optimized specifically for the **ZSH** shell environment, ensuring seamless command execution and styling for power users.*

## Features  
* **Arch Native**: Designed with the Arch Linux philosophy of simplicity and user-centricity.
* **ZSH Optimized**: Tailored to work out-of-the-box with ZSH workflows.
* **Media Amplified**: Built-in integration for media visualization and playback (FFmpeg & FFTW).
* **Parallel Workflow**: Spawn and manage multiple parallel tabs for complex multitasking.
* **Retro-Modern UI**: A minimalist, distraction-free interface with a classic digital vibe.

## Interface  
<div style="display: flex; overflow-x: auto; gap: 9px; padding: 0px; white-space: nowrap;">
  <img src="assets/visuals/0.png" alt="Visual 1" style="height: 200px; flex-shrink: 0;">
  <img src="assets/visuals/1.png" alt="Visual 2" style="height: 200px; flex-shrink: 0;">
  <img src="assets/visuals/2.png" alt="Visual 3" style="height: 200px; flex-shrink: 0;">
  <img src="assets/visuals/3.png" alt="Visual 4" style="height: 200px; flex-shrink: 0;">
  <img src="assets/visuals/4.png" alt="Visual 5" style="height: 200px; flex-shrink: 0;">
  <img src="assets/visuals/5.png" alt="Visual 6" style="height: 200px; flex-shrink: 0;">
</div>

## Dependencies  
### Framework  
**Qt 6**  
`sudo pacman -S qt6-base qt6-tools`

### Terminal Backend  
**QTermWidget**  
`sudo pamac build qtermwidget-git`  

### Media & Signal Processing  
**QMediaPlayer** (FFmpeg Option)  
`sudo pacman -S qt6-multimedia`  

**FFTW**  
`sudo pacman -S fftw`  

**FFmpeg**  
`sudo pacman -S ffmpeg`  

## Installation 
```
git clone https://github.com/user/Dause.git  
cd Dause  
mkdir build && cd build  
qmake6 ...  
make  
./dause
```  

## Shell Configuration (Optional)
If the Delete key on your physical keyboard removes entire words instead of single characters, add these lines to the bottom of your **~/.zshrc**:  
```
bindkey '^?' backward-delete-char  
bindkey '^H' backward-delete-char
```  