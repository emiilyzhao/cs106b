/*
 * File: PuzzleGUI.cpp
 * -------------------
 * @author Julie Zelenski, Spring 2023
 * Implementation of graphics/gui support for Tile Match.
 */
#include "PuzzleGUI.h"
#include "filelib.h"
#include "console.h"
#include "gconsolewindow.h"
#include "gbutton.h"
#include "gfilechooser.h"
#include "glabel.h"
#include "goptionpane.h"
#include "gthread.h"
#include "gwindow.h"
#include <QApplication>

/**
 *  __________________
 * < Here be dragons! >
 *  ------------------
 *                        \                    ^    /^
 *                         \                  / \  // \
 *                          \   |\___/|      /   \//  .\
 *                           \  /O  O  \__  /    //  | \ \           *----*
 *                             /     /  \/_/    //   |  \  \          \   |
 *                             @___@`    \/_   //    |   \   \         \/\ \
 *                            0/0/|       \/_ //     |    \    \         \  \
 *                        0/0/0/0/|        \///      |     \     \       |  |
 *                     0/0/0/0/0/_|_ /   (  //       |      \     _\     |  /
 *                  0/0/0/0/0/0/`/,_ _ _/  ) ; -.    |    _ _\.-~       /   /
 *                              ,-}        _      *-.|.-~-.           .~    ~
 *             \     \__/        `/\      /                 ~-. _ .-~      /
 *              \____(oo)           *.   }            {                   /
 *              (    (--)          .----~-.\        \-`                 .~
 *              //__\\  \__ Ack!   ///.----..<        \             _ -~
 *             //    \\               ///-._ _ _ _ _ _ _{^ - - - - ~
 *
 * Note to those delving into this code: writing a graphical application is hard.
 * It involves buttons, keyboard interaction, mouse interaction, windows interactions,
 * potentially reading and writing files, and interfacing with data structures from
 * the rest of the program.
 *
 * We've placed all of the messiness in this one file, and you are welcome to look through
 * it to see how it works. But, it isn't the prettiest 450 lines of code you'll ever read,
 * and it includes lots of low-level details that are certainly not required for someone
 * with a CS106B-level programming knowledge. If you do look through it, let us know, and
 * we're happy to discuss it in more detail one-on-one.
 **/

typedef Vector<Tile> Collection;

using namespace std;

static const double kTileSize = 150, kFrameWidth = 5;
static const string kSelectColor = "black", kMatchColor = "#008F00", kUnchanged = "unchanged";
static const int kNoSelection = -1;

// JDZ: module-private globals
static Action gAction;
static GWindow *gWin;
static GCanvas *gCanvas;
static Vector<GInteractor *> gControls;
struct TileInfo {
    struct {
        GImage *img;
        string sideStr;
    } rotations[NUM_SIDES];
    GCompound *compound;    // contains all rotations + frame
    GRect *frame;
    int index; // into stackinfo array
};
static Map<Tile, TileInfo> gTileInfo;
struct PlacementInfo {
    GPoint center;
    bool hasTile;
};
static Grid<PlacementInfo> gBoardInfo;
static Vector<PlacementInfo> gStackInfo;
static int gSelectedIndex = kNoSelection;

static bool readPuzzleConfigFile(string configFile, GridLocation& dim, Map<string,string>& pairs, Map<Tile, TileInfo>& tInfo);
static void resetLayout(int numRows = 3, int numCols = 3);
static void enableInteraction(Puzzle& puzzle, Collection& tiles);
static void disableInteraction();

static void updateKey(const Tile& tile, Collection* pCol = nullptr);
static GCompound *getTileGraphic(const Tile& tile, string frameColor = kUnchanged, GPoint* pCenter = nullptr);

bool loadPuzzleConfig(string configFile, Puzzle& puzzle, Collection& tiles) {
    GridLocation dim;
    Map<string,string> pairs;
    Map<Tile, TileInfo> tInfo;
    if (!readPuzzleConfigFile(configFile, dim, pairs, tInfo)) {
        if (!gWin) resetLayout();
        return false;
    }
    gTileInfo = tInfo;
    resetLayout(dim.row, dim.col);
    puzzle.configure(dim.row, dim.col, pairs);
    tiles.clear();
    for (const auto& cur: gTileInfo) { tiles.add(cur); }
    return true;
}

Action playInteractive(Puzzle& puzzle, Collection& tiles) {
    if (!gWin) resetLayout();
    updateDisplay(puzzle, tiles);
    enableInteraction(puzzle, tiles);
    gAction = NONE;
    while (gAction == NONE) { std::this_thread::yield(); }
    disableInteraction();
    return gAction;
}

void updateDisplay(const Puzzle& puzzle, const Collection& tiles, int pauseMs) {
    if (!gWin) error("TileGUI not properly initialized!");
    auto addTileAt = [&](Tile t, GPoint p, string fc) {
        GCompound *c = getTileGraphic(t, fc, &p);
        gCanvas->add(c);
    };
    gCanvas->clearObjects(); // remove all tiles from canvas
    for (auto& cur : gStackInfo) { cur.hasTile = false; }
    for (auto& cur : gBoardInfo) { cur.hasTile = false; }
    for (const auto& tile: tiles) { // iterate and set flag
        if (tile.isBlank()) continue;
        int index = gTileInfo[tile].index;
        gStackInfo[index].hasTile = true;
        updateKey(tile);
    }
    Vector<Tile> v = gTileInfo.keys();
    for (int i = gStackInfo.size() - 1; i >= 0; i--) {  // add in back-to-front z-order
        if (!gStackInfo[i].hasTile) continue;
        if (i == gSelectedIndex) continue; // add selected after loop so it will be in front
        addTileAt(v[i], gStackInfo[i].center, "");
    }
    if (gSelectedIndex != kNoSelection) {
        Tile which = v[gSelectedIndex];
        string frameColor = puzzle.canAdd(which) ? kMatchColor : kSelectColor;
        addTileAt(which, gStackInfo[gSelectedIndex].center, frameColor);
    }
    for (const auto& loc: gBoardInfo.locations()) {
        Tile tile = puzzle.tileAt(loc);
        if (tile.isBlank()) continue;
        addTileAt(tile, gBoardInfo[loc].center, "");
        gBoardInfo[loc].hasTile = true;
        updateKey(tile);
    }
    GThread::runOnQtGuiThread([] { gCanvas->repaint(); });
    if (pauseMs) pause(pauseMs);
}

static GCompound *getTileGraphic(const Tile& tile, string frameColor, GPoint* pCenter) {
    if (!gTileInfo.containsKey(tile)) error("TileGUI internal error missing image for tile!");
    TileInfo& info = gTileInfo[tile];
    string facing = tile.toString();
    for (int i = 0; i < NUM_SIDES; i++) {
        info.rotations[i].img->setVisible(info.rotations[i].sideStr == facing);
    }
    if (frameColor != kUnchanged) {
        info.frame->setVisible(!frameColor.empty());
        info.frame->setColor(frameColor);
    }
    if (pCenter) {
        // move all objects individually cause GCompound does not
        // draw offset as promised
        for (int i = 0; i < info.compound->getElementCount(); i++) {
            info.compound->getElement(i)->setCenterLocation(*pCenter);
        }
        info.compound->setCenterLocation(*pCenter);
    }
    return info.compound;
}

static void updateKey(const Tile& tile, Collection* pColl) {
    auto saved = gTileInfo[tile];   // JDZ ugly hack used to
    gTileInfo.remove(tile); // overwrite key with possibly rotated copy
    gTileInfo[tile] = saved;
    if (pColl) { // similar hack to replace entry in collection
        Collection copy;
        for (auto cur: *pColl) { copy.add(cur == tile ? tile : cur); }
        *pColl = copy;
    }
}

static bool getSelectedTile(Tile& tile) {
    if (gSelectedIndex == kNoSelection || !gStackInfo[gSelectedIndex].hasTile) return false;
    tile = gTileInfo.keys()[gSelectedIndex];
    return true;
}

static bool selectIndex(int newIndex, const Puzzle& puzzle) {
    for (const auto& cur : gTileInfo) gTileInfo[cur].frame->setVisible(false);
    gSelectedIndex = newIndex;
    Tile selected = gTileInfo.keys()[newIndex];
    string frameColor = puzzle.canAdd(selected) ? kMatchColor : kSelectColor;
    GCompound* c = getTileGraphic(selected, frameColor);
    //c->sendToFront(); // GCompound has sendToFront with argument that shadows inherited
    gCanvas->remove(c);   // corrected version operates manually
    gCanvas->add(c);
    GThread::runOnQtGuiThread([] { gCanvas->repaint(); });
    return true;
}

static bool up(const Puzzle& puzzle) {
    for (int i = gSelectedIndex + 1; i < gStackInfo.size(); i++) {
        if (gStackInfo[i].hasTile) return selectIndex(i, puzzle);
    }
    return false;
}

static bool down(const Puzzle& puzzle) {
    for (int i = gSelectedIndex - 1; i >= 0; i--) {
        if (gStackInfo[i].hasTile) return selectIndex(i, puzzle);
    }
    return false;
}

static bool rotate(int numTurns, const Puzzle& puzzle, Collection& tiles) {
    Tile tile;
    if (!getSelectedTile(tile)) return false;
    for (int i = 0; i < numTurns; i++) tile.rotate();
    string frameColor = puzzle.canAdd(tile) ? kMatchColor : kSelectColor;
    getTileGraphic(tile, frameColor);  // will change visibility of rotated images to match orientation
    updateKey(tile, &tiles);
    GThread::runOnQtGuiThread([] { gCanvas->repaint(); });
    return true;
}

static bool place(Puzzle& puzzle, Collection& tiles) {
    Tile which;
    if (!getSelectedTile(which) || !puzzle.canAdd(which)) return false;
    Collection without; // JDZ clumsy universal remove from either Set or Vector
    for (auto t: tiles) { if (!(t == which)) without.add(t);}
    tiles = without;
    puzzle.add(which);
    up(puzzle) || down(puzzle); // move select to neighbor tile in stack
    updateDisplay(puzzle, tiles); // redraw from model
    return true;
}

static bool remove(Puzzle& puzzle, Collection& tiles) {
    if (puzzle.isEmpty()) return false;
    Tile which = puzzle.remove();
    selectIndex(gTileInfo[which].index, puzzle);
    tiles.add(which);
    updateDisplay(puzzle, tiles); // redraw from model
    return true;
}

static bool handleClick(GEvent e, const Puzzle& puzzle) {
    // corrected version of hit test, need to search backwards (indexed z order background to foreground)
    GObject *hit = nullptr;
    for (int i = gCanvas->getElementCount()-1; i >= 0; i--) {
        GObject* cur = gCanvas->getElement(i);
        if (cur->contains(e.getX(), e.getY())) {
            hit = cur; break;
        }
    }
    if (!hit) return false;
    for (const auto& tile: gTileInfo) {
        if (gTileInfo[tile].compound == hit) {
            return selectIndex(gTileInfo[tile].index, puzzle);
        }
    }
    return false;
}

static bool handleKey(GEvent e, Puzzle& puzzle, Collection& tiles) {
    if (e.getEventType() != KEY_PRESSED) return true;
    switch (e.getKeyCode()) {
        case GEvent::LEFT_ARROW_KEY:   return rotate(3, puzzle, tiles);
        case GEvent::RIGHT_ARROW_KEY:  return rotate(1, puzzle, tiles);
        case GEvent::UP_ARROW_KEY:     return up(puzzle);
        case GEvent::DOWN_ARROW_KEY:   return down(puzzle);
        case GEvent::ENTER_KEY:        return place(puzzle, tiles);
        case GEvent::BACKSPACE_KEY:    return remove(puzzle, tiles);
        default:                       return false;
    }
}

static TileInfo createTileGraphic(string path, Tile tile) {
    TileInfo info;
    info.compound = new GCompound();
    // workaround to fix wrong compound bounds, add point in upperleft/lowerright
    info.compound->add(new GRect(0, 0, 0, 0));
    info.compound->add(new GRect(kTileSize, kTileSize, 0, 0));
    info.frame = new GRect(0, 0,  kTileSize, kTileSize);
    info.frame->setColor(kSelectColor);
    info.frame->setLineWidth(kFrameWidth);
    info.frame->setVisible(false);
    GImage *img = new GImage(path);
    img->setSize(kTileSize, kTileSize);
    // create 4 copies, one for each rotation, add to cache
    for (int orientation = 0; orientation < NUM_SIDES; orientation++) {
        GCanvas *offscreen = new GCanvas(kTileSize, kTileSize);
        // ideally would reuse single offscreen canvas, but internally sharing one QImage, ugh
        img->resetTransform();
        img->rotate(90 * orientation); // each rotate transforms 90 deg around top left
        if (orientation != 0) tile.rotate();
        switch (orientation) { // translate location to compensate
            case 0: img->setCenterLocation( kTileSize/2,  kTileSize/2); break;
            case 1: img->setCenterLocation( kTileSize/2, -kTileSize/2); break;
            case 2: img->setCenterLocation(-kTileSize/2, -kTileSize/2); break;
            case 3: img->setCenterLocation(-kTileSize/2,  kTileSize/2); break;
        }
        offscreen->draw(img);
        info.rotations[orientation].img = offscreen->toGImage();
        info.rotations[orientation].sideStr = tile.toString();
        info.compound->add(info.rotations[orientation].img);
    }
    info.compound->add(info.frame); // add frame last to put on top of image
    return info;
}

static void enableInteraction(Puzzle& puzzle, Collection& tiles) {
    for (int i = 0; i < gControls.size(); i++) {
        gControls[i]->setEnabled(true);
    }
    gCanvas->setClickListener([&puzzle](GEvent e)
        { handleClick(e, puzzle); });
    gCanvas->setKeyListener([&puzzle, &tiles](GEvent e)
        { if (!handleKey(e, puzzle, tiles)) QApplication::beep(); });
    gCanvas->requestFocus();
}

static void disableInteraction() {
    for (int i = 0; i < gControls.size(); i++) {
        gControls[i]->setEnabled(false);
    }
    gCanvas->removeClickListener();
    gCanvas->removeKeyListener();
    gSelectedIndex = kNoSelection;
}

static GWindow *createWindow() {
    static const string kWindowBackground = "#dddddd";
    GWindow *win = new GWindow();
    win->setLocation(0, 0);
    win->setTitle("Tile Match");
    win->setExitOnClose(true);
    win->setBackground(kWindowBackground);
    GLabel *insn = new GLabel("The selected tile is framed in black. Select a tile by clicking.\n"
        "Up/down arrow keys cycle selection through tiles. Left/right arrow keys rotate the selected tile.\n"
        "The frame of selected tile is highlighted in green when it can be added to board.\n"
        "Enter key adds the selected tile to board, Delete key removes the last tile added to board.");
    win->addToRegion(insn, GWindow::REGION_NORTH);
    gControls.add(insn);
    gCanvas = win->getCanvas();
    gControls.add(gCanvas);
    gCanvas->setAutoRepaint(false);
    auto addButton = [win](auto text, auto action) {
        GInteractor *b = new GButton(text);
        b->setActionListener(action);
        win->addToRegion(b, GWindow::REGION_SOUTH);
        gControls.add(b);
    };
    addButton("Load new puzzle", []() { gAction = LOAD_NEW; });
    addButton("Run my solver", []() { gAction = RUN_SOLVE; });
    return win;
}

static void drawBackground(GRectangle boardRect) {
    static const string kCanvasFill = "#bbbbbb";
    static const string kBoardFrame = "dark gray", kBoardFill = "#eeeeee";
    gCanvas->clearPixels();
    gCanvas->setBackground(kCanvasFill);
    GRect gr(0, 0, kTileSize, kTileSize);
    gr.setLineStyle(GObject::LINE_DOT);
    gr.setLineWidth(1);
    gr.setColor(kBoardFrame);
    gr.setFillColor(kBoardFill);
    for (const auto& loc: gBoardInfo.locations()) {
        gr.setCenterLocation(gBoardInfo[loc].center);
        gCanvas->draw(&gr);
    }
    gCanvas->setColor(kBoardFrame);
    gCanvas->setLineWidth(2);
    gCanvas->drawRect(boardRect);
}

static void resetLayout(int numRows, int numCols) {
    if (!gWin) gWin = createWindow();
    static const int kMargin = 10;
    GRectangle boardBounds = { kMargin, kMargin, numCols*kTileSize, numRows*kTileSize };
    GRectangle tileStackBounds = {0, 0, max(1.5, numCols/2.0) * kTileSize, max(1.5, numRows-0.5)*kTileSize};
    tileStackBounds.x = boardBounds.width + 3*kMargin;
    double maxHeight = max(boardBounds.height, tileStackBounds.height);
    tileStackBounds.y = (maxHeight- tileStackBounds.height)/2 + kMargin;
    boardBounds.y = (maxHeight- boardBounds.height)/2 + kMargin;
    gCanvas->setSize(boardBounds.width + tileStackBounds.width + 4*kMargin, maxHeight + 2*kMargin);
    gBoardInfo.resize(numRows, numCols);
    for (const auto& loc: gBoardInfo.locations()) {
        gBoardInfo[loc].center = { boardBounds.x + (loc.col * kTileSize) + kTileSize/2, boardBounds.y + (loc.row * kTileSize) + kTileSize/2 };
        gBoardInfo[loc].hasTile = false;
    }
    int numTiles = numRows*numCols;
    GPoint stackOffset = { (tileStackBounds.width-kTileSize)/(numTiles-1), (tileStackBounds.height-kTileSize)/(numTiles-1)};
    GPoint cur = {tileStackBounds.x + kTileSize/2, tileStackBounds.y + tileStackBounds.height - kTileSize/2};
    gStackInfo = Vector<PlacementInfo>(gTileInfo.size());
    for (auto& t : gStackInfo) {
        t.center = cur;
        cur.x += stackOffset.x;
        cur.y -= stackOffset.y;
    }
    drawBackground(boardBounds);
    gWin->setResizable(true);
    gWin->pack();
    gWin->setResizable(false);
    static const int kTitleBarHeight = 60, kConsoleHeight = 200;
    setConsoleLocation(0, gWin->getHeight() + kTitleBarHeight);
    setConsoleSize(gWin->getWidth(), kConsoleHeight);
}

static bool readPuzzleConfigFile(string configFile, GridLocation& dim, Map<string,string>& pairs, Map<Tile, TileInfo>& tInfo) {
    string reason;
    try {
        if (configFile.empty()) return false; // dialog canceled
        string dir = getHead(configFile);
        ifstream in;
        if (!openFile(in, configFile)) throw "No such file";
        auto readNext = [&in]() {
            string cur;
            do {
                if (!getline(in, cur)) break; trimInPlace(cur);
            } while (cur.empty() || startsWith(cur, "#"));
            return cur;
        };
        string line = readNext();
        istringstream stream(line);
        if (!(stream >> dim)) throw "First line does not contain dimensions, expected rNcN, found " + line;
        for (const auto& pair: stringSplit(readNext(), " ")) {
            Vector<string> tokens = stringSplit(pair, "=");
            if (tokens.size() != 2) throw "Malformed pair, expected format label=opposite, found " + pair;
            pairs[tokens[0]] = tokens[1]; // add self and inverse
            pairs[tokens[1]] = tokens[0];
        }
        string filename;
        while ((filename = readNext()) != "") {
            string path = dir + "/" + filename;
            string basename = getRoot(filename);
            if (!fileExists(path)) throw "No such image file: " + basename;
            Vector<string> edges = stringSplit(basename, "-");
            if (edges.size() < NUM_SIDES) throw "Tile image file name not in proper format, expected edges in N-E-S-W, found " + basename;
            Tile tile(edges[NORTH], edges[EAST], edges[SOUTH], edges[WEST]);
            if (tInfo.containsKey(tile)) throw "Duplicate tile listed twice: " + basename;
            for (Direction dir = NORTH; dir <= WEST; dir++) {
                if (!pairs.containsKey(tile.getEdge(dir))) throw "Edge label " + tile.getEdge(dir) + " of tile " + basename + " does not have matching entry in pairs";
            }
            tInfo[tile] = createTileGraphic(path, tile);
        }
        if (tInfo.size() != dim.row*dim.col) throw "Mismatch in size, dimensions = " + dim.toString() + " count of tiles = " + integerToString(tInfo.size());
        Vector<Tile> keys = tInfo.keys();
        for (int i = 0; i < keys.size(); i++) { tInfo[keys[i]].index = i; }
return true;
    } catch (const string& msg) {
        reason = msg;
    } catch (char const* msg) {
        reason = msg;
    } catch (ErrorException& ex) {
        reason = ex.getMessage();
    }
    string msg = "Error reading configuration file '" + getTail(configFile) + "'\nReason: " + reason;
    GOptionPane::showMessageDialog(msg, "Error", GOptionPane::MessageType::MESSAGE_ERROR);
    cerr << msg << endl;
    return false;
}

string chooseFileDialog() {
    return GFileChooser::showOpenDialog("Choose puzzle.txt config", "puzzles/", "*.txt");
}
