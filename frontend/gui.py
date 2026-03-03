import tkinter as Tk
from functools import partial
from ctypes import *


class EngineMove(Structure):
    _fields_ = [("origin", c_int),
                ("target", c_int),
                ("side", c_int),
                ("piece", c_int),
                ("is_ep", c_bool),    
                ("castling", c_int),            
                ]
    
    def to_sqr_centric(self):
        src    =  (int(self.origin/8), self.origin % 8)
        target =  (int(self.target/8), self.target % 8)
        return (src, target)


class ChessGUI:
    engine = None
    window = None
    board = [[None for col in range(8)] for row in range(8)]
    board_colors = tuple()
    piece_imgs = dict()      # dict of Tk.PhotoImage instances
    selected_btn = None     # (btn, row, col)
    turn = "white"
    ai_player = "black"
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
    btn_info = dict()


    def __init__(self, engine, depth:int, window_name:str, window_size:str, board_colors:tuple):
        self.engine = engine
        self.engine.search_move.restype = EngineMove
        self.engine.search_move.argtypes = [c_int]
        self.engine.make_move.restype = c_bool
        self.engine.make_move.argtypes = [c_void_p]
        self.depth = depth
        self.engine.engine_init()
        self.board_colors = board_colors
        self.setup_board()
        self.create_window(window_name, window_size)
        self.window.mainloop()


    # Setup board to be in initial chess position
    def setup_board(cls):
        for col in range(8):
            cls.board[1][col] = "white_pawn"
            cls.board[6][col] = "black_pawn"
        cls.board[0][0] = "white_rook"
        cls.board[7][0] = "black_rook"
        cls.board[0][1] = "white_knight"
        cls.board[7][1] = "black_knight"
        cls.board[0][2] = "white_bishop"
        cls.board[7][2] = "black_bishop"
        cls.board[0][3] = "white_queen"
        cls.board[7][3] = "black_queen"
        cls.board[0][4] = "white_king"
        cls.board[7][4] = "black_king"
        cls.board[0][5] = "white_bishop"
        cls.board[7][5] = "black_bishop"
        cls.board[0][6] = "white_knight"
        cls.board[7][6] = "black_knight"
        cls.board[0][7] = "white_rook"
        cls.board[7][7] = "black_rook"
        

    def create_window(cls, window_name:str, window_size:str):
        cls.window = Tk.Tk()
        cls.window.title(window_name)
        cls.window.geometry(f"{window_size}x{window_size}")
        cls.window.rowconfigure(tuple(range(8)), weight=1)
        cls.window.columnconfigure(tuple(range(8)), weight=1)
        cls.create_buttons()


    def create_buttons(cls):
        for row in range(8):
            for col in range(8):
                
                sqr_color = cls.get_sqr_color(row, col)
                btn = Tk.Button(cls.window, bg=sqr_color, activebackground=sqr_color, cursor="hand2")
                btn.config(command= partial(cls.btn_clicked, row,col,btn))
                btn.grid(row=7-row, column=col, sticky="nwes")
                piece = cls.board[row][col]
    
                if piece not in cls.piece_imgs:
                    img=Tk.PhotoImage(file=cls.img_paths[piece], width=60, height=60)
                    cls.piece_imgs[piece] = img

                btn.config(image=cls.piece_imgs[piece])

                cls.btn_info[(row,col)] = btn


    # Handler for button click event
    def btn_clicked(cls, row, col, btn):

        if cls.turn == cls.get_piece_color(row,col):
            cls.select(btn,row,col)
        elif cls.selected_btn is not None:
            move = (cls.selected_btn[1:], (row,col))
            src = 8*move[0][0] + move[0][1]
            target = 8*move[1][0] + move[1][1]

            
            res = cls.engine.find_n_play(src, target)
            if res == False: # Move illegal
                return

            cls.play_move(move)
            cls.window.update()
            if cls.turn == cls.ai_player:
                move = cls.engine.search_move(cls.depth)
                src,target = move.to_sqr_centric()
                cls.play_move((src,target))
                cls.engine.make_move(addressof(move))


    def play_move(cls, move):
        src,target = move
        cls.board[target[0]][target[1]] = cls.board[src[0]][src[1]]
        cls.board[src[0]][src[1]] = None
        cls.pass_turn()
        cls.render_board()
        cls.check_game_over()


    def check_game_over(cls):
        result = cls.engine.game_result()
        if result == 0:
            print("WHITE WON")
            exit(0)
        elif result == 1:
            print("BLACK WON")
            exit(0)
        elif result == 2:
            print("DRAW")
            exit(0)



    def render_board(cls):
        for row in range(8):
            for col in range(8):
                piece = cls.board[row][col]
                btn = cls.btn_info[(row,col)]
                btn.config(image=cls.piece_imgs[piece])
                    

    def pass_turn(cls):
        if cls.selected_btn is not None:
            cls.deselect()
        cls.turn = "white" if cls.turn == "black" else "black"

    def select(cls, btn:Tk.Button, row:int, col:int):
        if cls.selected_btn is not None:
            cls.deselect()
        btn.config(bg="yellow", activebackground="yellow")
        cls.selected_btn = (btn, row, col)

    def deselect(cls):
        color = cls.get_sqr_color(*cls.selected_btn[1:])
        cls.selected_btn[0].config(bg=color, activebackground=color)
        cls.selected_btn = None

    def get_sqr_color(cls, row:int, col:int):
        return cls.board_colors[0] if (row+col)%2 == 0 else cls.board_colors[1]
    
    def get_piece_color(cls, row:int, col:int):
        piece = cls.board[row][col]
        return None if piece is None else piece.split("_")[0]