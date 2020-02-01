# Tab Appraiser
Tab Appraiser is an overlay stash tab price checker for the game [Path of Exile](https://www.pathofexile.com/).

The overlay uses the [pathofexile.com](https://www.pathofexile.com/) and [poe.ninja](poe.ninja) APIs to check item prices of a stash tab, displaying the information on top of the Path of Exile application.

## Notes
- The program only supports price checking of items with static values. Prices for Rare items, 6-links, skill gems, etc. are not supported.
- The program requires a POESESSID in order to get private stash tab information. A guide to find your POESESSID is located below.
 
## Download and Run
1. Go to the [Releases](https://github.com/Nickswoboda/tab-appraiser/releases) section.
2. Download the TabAppraiser.zip file.
3. Extract the .zip file to your desired location.
4. Run the .exe file

In order for the overlay to be displayed, the Path of Exile game must not be in full screen mode.

## How to find your POESESSID
Go to https://www.pathofexile.com/ and log in to your account.
### Chrome
1. Press F12.
2. Select "Application" in the top toolbar.
3. Expand the "Cookies" dropdown in the side bar, and click https://www.pathofexile.com/.
4. Copy the value associated with "POESESSID".
### Firefox
1. Press F12.
2. Select "Storage in the top toolbar.
3. Expand the "Cookies" dropdown in the side bar, and click https://www.pathofexile.com/.
4. Copy the value associated with "POESESSID".


## System Requirements
OpenGL 3.0

## Special Thanks
Thank you to Path of Exile and poe.ninja for their APIs that make this program possible.
