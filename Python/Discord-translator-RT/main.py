##########################################################
#   Overlaps a transparent tkinter window on Discord
#   Remove the right border + exteriors to change Discord
#       window size
#   Can click inside the invisible ovelay to interact
#       with lower windows
#   Todo: Add the translation part
##########################################################
from tkinter import *
import pygetwindow as gw


def get_Discord_position():
    window = gw.getWindowsWithTitle('Discord')[0]
    return window

def get_location_Discord(window_settings):
    size_left_menu = 310
    size_border = 28

    location = str(window_settings.width - size_left_menu) + "x" + \
               str(window_settings.height - size_border) + "+" + \
               str(window_settings.left + size_left_menu) + "+" + \
               str(window_settings.top + size_border)

    return location

root = Tk()

label = Label(root, text="This is a New Line Text", font='Helvetica 12 bold', foreground="black")
label.place(x=0, y=0)


root.wm_attributes('-transparentcolor', '#add123')      # Create a transparent window
root.wm_attributes("-topmost", True)
root.overrideredirect(True)

root.geometry("10x10")    # Set the geometry of window
root.config(bg='#add123')   # Add a background color to the Main Window

while True:
    discord_window = get_Discord_position()
    overlay_geometry = get_location_Discord(discord_window)
    root.geometry(overlay_geometry)
    # label.place(x=150, y=50)
    root.update()
