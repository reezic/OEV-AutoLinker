## Obsidian Entrance Visualizer for SoH
This build of Ship of Harkinian automatically creates discovered entrances as linked markdown files which can be viewed as a node map in Obsidian, resulting in an evolving map of the game as you play.<br>

<br>

If you have any questions about Ship of Harkinian, and how to get started, you can read more on [the project's homepage](https://www.shipofharkinian.com/)<br>
Official releases can be found at [SoH's GitHub](https://github.com/HarbourMasters/Shipwright)

<br>

<table>
  <tr>
    <th colspan="2">No randomization</th>
  </tr>
  <tr>
    <td colspan="2">
      <p align="center">
        <img src="https://github.com/user-attachments/assets/ee416ccb-8728-4a2a-97bc-3badffd67a4d" width="400">
      </p>
    </td>
  </tr>
  <tr>
    <th>Randomized</th>
    <th>Randomized (decoupled)</th>
  </tr>
  <tr>
    <td>
      <img src="https://github.com/user-attachments/assets/20837f85-560a-4481-a62b-e2c79dc40f35" width="600">
    </td>
    <td>
      <img src="https://github.com/user-attachments/assets/774b307f-a21e-41ae-9250-a91c8c18f970" width="600">
    </td>
  </tr>
</table>


_Sample maps with various randomizer settings._

<br>

## How does it work?
* [Download the latest release of OEV](https://github.com/reezic/OEV-AutoLinker/releases/tag/1.5.2)

* [Download Obsidian](https://obsidian.md/)

* A folder named "Obsidian Entrance Visualizer" is included in the root folder of the release which contains an Obsidian vault for each save slot in the game

* In Obsidian, open the vault matching the save slot you intend to use, and open the graph view

* Start a new save in OoT

* Watch as Obsidian's graph view automatically creates a map of the game's world as you explore it!

#### _Notes_

_Saves from other builds of SoH will not work with the AutoLinker functionality._

<br>

## Recommended plugins
I highly recommend these plugins for Obsidian to make the experience better:

[libasoles' unofficial "Graph View Text Size" plugin](https://github.com/libasoles/graph-view-text-size) (Included in this release of OEV Autolinker)<br>
Since this is an unofficial plugin requiring you to follow the build instructions on libasoles' GitHub, I've already included it in the latest release of OEV Autolinker.

This plugin allows you to change the text size and color of the node text so that it matches with the color of the node itself, making the map more readable.

<br>

If nodes aren't updating with the correct styles, just click one of them and they should refresh.

I recommend pinning the graph view (right click the tab in Obsidian) then dragging the tab with the clicked node's contents to one side so that you have the node content and the graph in a split pane view to minimize the disruption of clicking on nodes to refresh the graph.

<br>

[Sanqui's "Obsidian Persistent Graph Plugin"](https://github.com/libasoles/graph-view-text-size)<br>
This plugin lets you save node positions in the graph view.

To save your node layout, press CRTL+P, then search for "save" and click "Persistent Graph: Save graph node positions".<br>
To load saved node layouts click "Persistent Graph: Restore graph node locations".
