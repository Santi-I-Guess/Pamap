from random import randint, sample, shuffle
from string import ascii_lowercase, ascii_uppercase
import sys


class coordinate:
    def __init__(self):
        self.space: list[int] = [randint(-100, 100) for i in range(3)]
        self.edges: list[str] = []


class input_properties:
    def __init__(self):
        letters = ascii_lowercase + ascii_uppercase
        aux_symbols = [i for i in letters]
        shuffle(aux_symbols)
        self.num_trails: int = randint(2, 4)
        self.num_edges: int = randint(1, 12)
        self.symbols: str = "".join(aux_symbols[:self.num_edges])
        self.trails: list[list[coordinate]] = [
            [] for i in range(self.num_trails)
        ]
        self.generate_trails()
        self.spread_edges()

    def generate_trails(self) -> None:
        for trail_num in range(self.num_trails):
            self.trails[trail_num] = [
                coordinate() for i in range(randint(1, 32))
            ]

    def spread_edges(self) -> None:
        # ensure every edge symbol occurs exactly twice in a file
        # ensure every edge symbol pair is split between two unique trails
        for edge_symbol in self.symbols:
            first_idx, second_idx = sample(range(self.num_trails), 2)
            first_coord_idx = randint(
                0, len(self.trails[first_idx])-1
            )
            second_coord_idx = randint(
                0, len(self.trails[second_idx])-1
            )
            self.trails[first_idx][first_coord_idx].edges += edge_symbol
            self.trails[second_idx][second_coord_idx].edges += edge_symbol

    def print_form(self, le_file) -> None:
        for curr_trail in self.trails:
            print("[", file=le_file)
            for curr_coord in curr_trail:
                if len(curr_coord.edges) == 0:
                    curr_edge_form = ""
                else:
                    curr_edge_form = "{" \
                        + "".join(sorted(curr_coord.edges)) + "}"
                curr_coord_form = "(" + ",".join(
                    str(i) for i in curr_coord.space
                ) + ")"
                print(f"    {curr_edge_form}{curr_coord_form},", file=le_file)
            print("]", file=le_file)


sink_folder_path = sys.argv[1]
for file_idx in range(16):
    current = input_properties()
    sink = open(f"{sink_folder_path}/input_{file_idx:0>2}.txt", "w")
    current.print_form(sink)
    sink.close()
    # 1. spread symbol pairs between trails
    # 2. spread symbols among coordinates
    # 3. format, and print to example input file


# every trail has square brackets
# every coordinate has parenthesis, 3 values, 2 commas
# every edge symbol has curly braces, seperated by commas, in [a-zA-Z]
# every edge symbol must be followed by curly braces
# edge symbols must come in pairs, each symbol exactly once
# edge symbols must be on different trails (implying 2+ trails exist)
