##########################################################
#    Overlaps a transparent tkinter window on Discord
#
##########################################################
import tkinter as tk
import pygetwindow as gw
import pyautogui
import cv2
import numpy as np

root = tk.Tk()
text_to_display = "Hello, Transparent Window!"

window_width = 400
window_height = 100

def on_after():
    label.configure(text="hello")

if __name__ == '__main__':

    discord_window = gw.getWindowsWithTitle('Discord')[0]
    print(discord_window)
    """
    screenshot = pyautogui.screenshot(region=(discord_window.left, discord_window.top, discord_window.width, discord_window.height))
    img_np = np.array(screenshot)
    cv2.imwrite('processed_image_.png', img_np)
    """

    root.image = tk.PhotoImage(file='butterfly.gif')    # The image must be stored to Tk or it will be garbage collected.
    root.configure(bg='')
    label = tk.Label(root, text=text_to_display, font=('Arial', 10), fg='black')    # fg='white', bg='black'
    label = label.place(x=0, y=0)
    root.overrideredirect(True)
    root.geometry(f"{discord_window.width}x{discord_window.height}+{discord_window.left}+{discord_window.top}")  # real 1920x1080    # 800x500+250+250
    root.lift()
    root.wm_attributes("-topmost", True)
    root.wm_attributes("-disabled", True)
    root.wm_attributes("-transparentcolor", "white")

    while True:
        discord_window = gw.getWindowsWithTitle('Discord')[0]
        root.geometry(f"{discord_window.width}x{discord_window.height}+{discord_window.left}+{discord_window.top}")
        root.update()
