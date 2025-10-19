import tkinter as Tk
from functools import partial


class ChessGUI:

    window = None
    board_colors = tuple()
    piece_imgs = set()
    img_paths = {
        "white_pawn": "frontend/images/white_pawn.png",
        "black_pawn": "frontend/images/black_pawn.png",
        "white_rook": "frontend/images/white_rook.png",
        "black_rook": "frontend/images/black_rook.png",
        "white_bishop": "frontend/images/white_bishop.png",
        "black_bishop": "frontend/images/black_bishop.png",
        "white_knight": "frontend/images/white_knight.png",
        "black_knight": "frontend/images/black_knight.png",
        "white_king": "frontend/images/white_king.png",
        "black_king": "frontend/images/black_king.png",
        "white_queen": "frontend/images/white_queen.png",
        "black_queen": "frontend/images/black_queen.png",
        None: ""
    } 


    def __init__(self, window_name:str, window_size:str, board_colors:tuple):
        self.board_colors = board_colors
        self.create_window(window_name, window_size)
        self.window.mainloop()


    def create_window(cls, window_name:str, window_size:str):
        cls.window = Tk.Tk()
        cls.window.title(window_name)
        cls.window.geometry(f"{window_size}x{window_size}")
        cls.window.rowconfigure(tuple(range(8)), weight=1)
        cls.window.columnconfigure(tuple(range(8)), weight=1)

        # Setup pieces
        for row in range(8):
            for col in range(8):

                piece = None
                if row == 1 or row == 6:
                    piece = "_pawn"
                elif row == 0 or row == 7:
                    if col == 0 or col == 7:
                        piece = "_rook"
                    elif col == 1 or col == 6:
                        piece = "_knight"
                    elif col == 2 or col == 5:
                        piece = "_bishop"
                    elif col == 3:
                        piece = "_queen"
                    elif col == 4:
                        piece = "_king"

                if piece is not None:
                    if row < 4:
                        piece = f"white{piece}"
                    else:
                        piece = f"black{piece}"
                
                sqr_color = cls.get_sqr_color(row, col)
                btn = Tk.Button(cls.window, bg=sqr_color, activebackground=sqr_color, pady=23, cursor="hand2", command= partial(cls.btn_clicked, row,col))
                btn.grid(row=7-row, column=col, sticky="nwes")
                if piece != None:
                    img=Tk.PhotoImage(file=cls.img_paths[piece])
                    cls.piece_imgs.add(img)
                    btn.config(image=img)

    
    def btn_clicked(cls, row, col):
        print(row, col)


    def get_sqr_color(cls, row:int, col:int):
        return cls.board_colors[0] if (row+col)%2 == 0 else cls.board_colors[1]