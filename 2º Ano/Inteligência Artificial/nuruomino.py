# nuruomino.py: Template para implementação do projeto de Inteligência Artificial 2024/2025.
# Devem alterar as classes e funções neste ficheiro de acordo com as instruções do enunciado.
# Além das funções e classes sugeridas, podem acrescentar outras que considerem pertinentes.

# Grupo 53:
# 57175 João Pedro dos Santos Carvalho
# 107032 Rodrigo Alexandre Almeida Fonseca dos Santos

from sys import stdin
import numpy as np
from search import *

def compute_orientations(piece: np.ndarray) -> list[np.ndarray]:
    piece_orientations = []
    seen = set()
    for k in range(4):
        rotated_piece = np.rot90(piece, k)
        for flip in [True, False]:
            oriented_piece = np.fliplr(rotated_piece) if flip else rotated_piece
            # Use tuple for faster comparison
            oriented_tuple = tuple(map(tuple, oriented_piece))
            if oriented_tuple not in seen:
                seen.add(oriented_tuple)
                piece_orientations.append(oriented_piece)
    return piece_orientations

# Global definitions of masks, representing pieces
BASE_PIECES = {'L': np.array([[1, 0], 
                              [1, -1],
                              [1, 1]], dtype=np.int8),
               'I': np.array([[1],
                              [1],
                              [1],
                              [1]], dtype=np.int8),
               'T': np.array([[1, 1, 1],
                              [-1, 1, -1]], dtype=np.int8),
               'S': np.array([[-1, 1, 1],
                              [1, 1, -1]], dtype=np.int8)}
    
PIECE_MASKS = {}              
for piece, mask in BASE_PIECES.items():
    PIECE_MASKS[piece] = compute_orientations(mask)

class Action:
    def __init__(self, region_num, row, col, piece_name, piece_mask, affected_regions=0):
        self.region_num = region_num
        self.row = row
        self.col = col
        self.piece_name = piece_name
        self.piece_mask = piece_mask
        self.affected_regions = affected_regions  # Para least-constraining value
        
        # Células preenchidas, pré-calculadas uma vez (piece_mask é tuplo de tuplos)
        self.cells = frozenset((row + i, col + j)
                               for i, mrow in enumerate(piece_mask)
                               for j, v in enumerate(mrow) if v == 1)

    def __iter__(self):
        return iter((self.region_num, self.row, self.col, self.piece_name, self.piece_mask))

    def __hash__(self):
        return hash((self.region_num, self.row, self.col, self.piece_name, self.piece_mask))

    def __eq__(self, other):
        return (self.region_num, self.row, self.col, self.piece_name, self.piece_mask) == \
               (other.region_num, other.row, other.col, other.piece_name, other.piece_mask)

class NuruominoState:
    def __init__(self, placed_pieces=frozenset(), region_domains=None):
        self.placed_pieces = placed_pieces
        self.region_domains = region_domains  # dict {região: set de ações}
        self._hash = None

    def __lt__(self, other):
        return len(self.placed_pieces) < len(other.placed_pieces)

    def __hash__(self):
        if self._hash is None:
            self._hash = hash(self.placed_pieces)
        return self._hash

    def __eq__(self, other):
        return isinstance(other, NuruominoState) and self.placed_pieces == other.placed_pieces

class Board:
    def __init__(self, dim, num_regions, region_grid, region_info):
        self.dimensions = dim
        self.num_regions = num_regions
        self.region_grid = region_grid
        self.region_info = region_info
        self.directions = [(0, 1), (0, -1), (1, 0), (-1, 0)]
        self._compute_region_adjacencies()
        self.region_domains = self._precompute_region_domains()
    
    def _precompute_region_domains(self):
        domains = {}
        for r in range(1, self.num_regions + 1):
            domains[r] = self.get_possible_placements(r, frozenset())
        return domains

    def _compute_region_adjacencies(self):
        self.region_adjacencies = {}
        for r in range(1, self.num_regions + 1):
            adjacent_regions = set()
            for i, j in self.region_info[r]['cells']:
                for di, dj in self.directions:
                    ni, nj = i + di, j + dj
                    if (0 <= ni < self.dimensions and 0 <= nj < self.dimensions):
                        adj_region = self.region_grid[ni, nj]
                        if adj_region != r:
                            adjacent_regions.add(adj_region)
            self.region_adjacencies[r] = adjacent_regions
    
    def print_solution(self, placed_pieces):
        display_grid = np.copy(self.region_grid).astype(object) # copy region grid and allow entry dtypes other than int8
        for region_num, row, col, piece_name, piece_mask in placed_pieces:
            piece_mask = np.array(piece_mask)
            for i in range(piece_mask.shape[0]):
                for j in range(piece_mask.shape[1]):
                    if piece_mask[i, j] == 1:
                        display_grid[row + i, col + j] = piece_name
        
        for i in range(self.dimensions):
            print('\t'.join(str(display_grid[i, j]) for j in range(self.dimensions)))
    
    @staticmethod
    def parse_instance():
        line = stdin.readline().split()
        if not line:
            raise EOFError("Input is empty or not formatted correctly.")
        
        dim = len(line)
        region_grid = np.zeros((dim, dim), dtype=np.int8)
        num_regions = 0
        
        # Read the first line
        for j, region_num in enumerate(line):
            r = int(region_num)
            num_regions = max(num_regions, r)
            region_grid[0, j] = r
        
        # Read remaining lines
        for i in range(1, dim):
            line = stdin.readline().split()
            for j, region_num in enumerate(line):
                r = int(region_num)
                num_regions = max(num_regions, r)
                region_grid[i, j] = r
        
        # Compute region information
        region_info = {}
        for r in range(1, num_regions + 1):
            cells = set()
            min_row = min_col = dim
            max_row = max_col = -1
            
            for i in range(dim):
                for j in range(dim):
                    if region_grid[i, j] == r:
                        cells.add((i, j))
                        min_row = min(min_row, i)
                        max_row = max(max_row, i)
                        min_col = min(min_col, j)
                        max_col = max(max_col, j)
            
            region_info[r] = {
                'cells': cells,
                'bounds': (min_row, max_row, min_col, max_col)
            }
        
        return Board(dim, num_regions, region_grid, region_info)
    
    def get_occupied_cells(self, placed_pieces):
        """Get all cells occupied by placed pieces."""
        occupied = set()
        blocked = set()  # cells that are blocked by piece corners
        
        for _, row, col, _, piece_mask in placed_pieces:
            piece_mask = np.array(piece_mask)
            for i in range(piece_mask.shape[0]):
                for j in range(piece_mask.shape[1]):
                    if piece_mask[i, j] == 1:
                        occupied.add((row + i, col + j))
                    elif piece_mask[i, j] == -1:
                        blocked.add((row + i, col + j))
        
        return occupied, blocked
    
    def is_valid_placement(self, region_num, row, col, piece_name, piece_mask, placed_pieces):
        piece_mask = np.array(piece_mask)
        piece_height, piece_width = piece_mask.shape
        
        # Check if in-bounds
        if row + piece_height > self.dimensions or col + piece_width > self.dimensions:
            return False
        
        region_cells = self.region_info[region_num]['cells']
        piece_cells = set()
        invalid_cells = set()
        
        # Get piece and invalid cells
        for i in range(piece_height):
            for j in range(piece_width):
                cell_row, cell_col = row + i, col + j
                if piece_mask[i, j] == 1:
                    piece_cells.add((cell_row, cell_col))
                elif piece_mask[i, j] == -1:
                    invalid_cells.add((cell_row, cell_col))
        
        # Check if piece cells belong to the region
        if not piece_cells.issubset(region_cells):
            return False
        
        # Get currently occupied cells
        occupied, blocked = self.get_occupied_cells(placed_pieces)
        
        # Check for overlaps
        if piece_cells & occupied or invalid_cells & (occupied | blocked):
            return False
        
        # Look for touching pieces of the same kind
        for p_row, p_col in piece_cells:
            for di, dj in self.directions:
                adj_row, adj_col = p_row + di, p_col + dj
                if (adj_row, adj_col) in occupied:
                    # Find which piece occupies this adjacent cell
                    for _, pp_row, pp_col, pp_name, pp_mask in placed_pieces:
                        if pp_name == piece_name:
                            pp_mask = np.array(pp_mask)
                            for n in range(pp_mask.shape[0]):
                                for m in range(pp_mask.shape[1]):
                                    if (pp_mask[n, m] == 1 and 
                                        pp_row + n == adj_row and pp_col + m == adj_col):
                                        return False
        return True
    
    def get_possible_placements(self, region_num, placed_pieces):
        # Check if region already has a piece
        if any(p[0] == region_num for p in placed_pieces):
            return []
        
        valid_placements = []
        region_bounds = self.region_info[region_num]['bounds']
        min_row, max_row, min_col, max_col = region_bounds
        
        for piece_name, orientations in PIECE_MASKS.items():
            for piece_mask in orientations:
                piece_height, piece_width = piece_mask.shape
                
                # Limit search area to region bounds
                start_row = max(0, min_row - piece_height + 1)
                end_row = min(self.dimensions - piece_height + 1, max_row + 1)
                start_col = max(0, min_col - piece_width + 1)
                end_col = min(self.dimensions - piece_width + 1, max_col + 1)
                
                for row in range(start_row, end_row):
                    for col in range(start_col, end_col):
                        if self.is_valid_placement(region_num, row, col, piece_name, piece_mask, placed_pieces):
                            # Compute afected neighbour regions
                            affected = set()
                            for i in range(piece_height):
                                for j in range(piece_width):
                                    if piece_mask[i, j] == 1:
                                        for di, dj in self.directions:
                                            ni, nj = row + i + di, col + j + dj
                                            if (0 <= ni < self.dimensions and 0 <= nj < self.dimensions):
                                                adj_region = self.region_grid[ni, nj]
                                                if adj_region != region_num:
                                                    affected.add(adj_region)
                            mask_tuple = tuple(map(tuple, piece_mask))
                            action = Action(region_num, row, col, piece_name, mask_tuple, len(affected))
                            valid_placements.append(action)
        return valid_placements

    def is_complete(self, placed_pieces): # i.e. all regions have pieces
        placed_regions = {p.region_num for p in placed_pieces}
        return len(placed_regions) == self.num_regions
    
    def has_squares(self, placed_pieces):
        occupied, _ = self.get_occupied_cells(placed_pieces)
        
        for i in range(self.dimensions - 1):
            for j in range(self.dimensions - 1):
                square_cells = [(i, j), (i, j+1), (i+1, j), (i+1, j+1)]
                if all(cell in occupied for cell in square_cells):
                    return True
        return False
    
    def check_piece_contiguity(self, placed_pieces):
        if not placed_pieces:
            return True
        
        occupied, _ = self.get_occupied_cells(placed_pieces)
        if not occupied:
            return True
        
        # BFS to check contiguity
        visited = set()
        queue = [next(iter(occupied))]
        
        while queue:
            row, col = queue.pop(0)
            if (row, col) in visited:
                continue
            visited.add((row, col))
            
            for di, dj in self.directions:
                ni, nj = row + di, col + dj
                if (ni, nj) in occupied and (ni, nj) not in visited:
                    queue.append((ni, nj))
        
        return len(visited) == len(occupied)

class Nuruomino(Problem):
    def __init__(self, board: Board):
        self.board = board
        # Estado inicial: nada colocado. A propagação de forçadas passou para
        # dentro de actions()
        self.initial = NuruominoState(placed_pieces=frozenset(), region_domains=None)
                
    def actions(self, state: NuruominoState):
        placed = state.placed_pieces
        placed_regions = {p.region_num for p in placed}
        unassigned_regions = [r for r in range(1, self.board.num_regions + 1)
                                if r not in placed_regions]
        if not unassigned_regions:
            return []
        # Ocupadas + mapa célula->nome da peça, construídos uma vez com as
        # células pré-calculadas
        occupied = set()
        cell_name = {}
        for a in placed:
            for c in a.cells:
                occupied.add(c)
                cell_name[c] = a.piece_name
        dirs = self.board.directions

        def local_ok(a):
            cells = a.cells
            # sem quadrados 2x2 preenchidos (só à volta desta peça)
            for (r, c) in cells:
                for dr in (0, -1):
                    for dc in (0, -1):
                        sq = ((r+dr, c+dc), (r+dr+1, c+dc),
                              (r+dr, c+dc+1), (r+dr+1, c+dc+1))
                        if all((x in occupied or x in cells) for x in sq):
                            return False
            # Não toca em peça igual
            for(r, c) in cells:
                for dr, dc in dirs:
                    nb = (r+dr, c+dc)
                    if nb in occupied and cell_name[nb] == a.piece_name:
                        return False
            return True
        # Forward-check: cada região por atribuir tem de manter >=1 colocação válida
        region_valid = {}
        for r in unassigned_regions:
            valid = [a for a in self.board.region_domains[r] if local_ok(a)]
            if not valid:
                return []
            region_valid[r] = valid

        # Unit propagation: região com uma única colocação válida é forçada
        # (está em qualquer solução) -> coloca-se já, sem ramificar, mesmo que
        # ainda não toque no bloco. Colapsa regiões pequenas (ex.: tamanho 4)
        for r in unassigned_regions:
            if len(region_valid[r]) == 1:
                return region_valid[r]
        
        # Primeira peça: semear a partir da região mais constrangida (MRV)
        if not occupied:
            r = min(unassigned_regions, key=lambda r: len(region_valid[r]))
            return sorted(region_valid[r], key=lambda a: a.affected_regions)
        
        # Peças seguintes: só colocações que tocam no bloco já montado
        # (mantém o preenchido ligado por construção). Consideram-se as de
        # todas as regiões adjacentes -- restringir a uma só perderia completude
        def touches(a):
            for (r, c) in a.cells:
                for dr, dc in dirs:
                    if (r+dr, c+dc) in occupied:
                        return True
            return False
        
        touching = [a for r in unassigned_regions for a in region_valid[r] if touches(a)]
        if not touching:
            return []           # bloco nao cresce mas faltam regiões

        # Ordena por região mais constrangida primeiro, depois menos-restritiva
        return sorted(touching, key=lambda a: (len(region_valid[a.region_num]), a.affected_regions))



    def result(self, state: NuruominoState, action):
        # actions() valida tudo contra board.region_domains, por isso o estado
        # só precisa do conjunto de peças colocadas (sem cópia de domínios por nó)
        return NuruominoState(state.placed_pieces | {action}, None)

    def goal_test(self, state: NuruominoState):
        """Check if state is a goal state."""
        return (self.board.is_complete(state.placed_pieces) and
                not self.board.has_squares(state.placed_pieces) and
                self.board.check_piece_contiguity(state.placed_pieces))
       
if __name__ == "__main__":
    board = Board.parse_instance()
    prob = Nuruomino(board)
    # Use depth-first search instead of A* for better memory usage
    sol = depth_first_graph_search(prob)
    if sol is not None:
        board.print_solution(sol.state.placed_pieces)
    else:
        print('No solution found.')