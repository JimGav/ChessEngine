import ctypes as ct
from frontend.gui import ChessGUI

if __name__ == "__main__":
    
    engine = ct.cdll.LoadLibrary("./backend/lib/engine.so")
    gui = ChessGUI(engine, 2, "stathis 2.0", 600, ["blue", "white"])