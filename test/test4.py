class Node:
    pass

class Var(Node):
    def __init__(self, name):
        self.name = name

class Assign(Node):
    def __init__(self, var, expr):
        self.var = var
        self.expr = expr

class BinOp(Node):
    def __init__(self, left, op, right):
        self.left = left
        self.op = op
        self.right = right

class Program(Node):
    def __init__(self, statements):
        self.statements = statements

import re

def parse(code):
    statements = []
    for line in code.splitlines():
        line = line.strip()
        if line:
            match = re.match(r'(\w+) = (.+)', line)
            if match:
                var_name = match.group(1)
                expr = match.group(2)
                # For simplicity, we only handle binary operations
                if '+' in expr:
                    left, right = expr.split('+')
                    statements.append(Assign(Var(var_name), BinOp(Var(left.strip()), '+', Var(right.strip()))))
                else:
                    statements.append(Assign(Var(var_name), Var(expr.strip())))
    return Program(statements)

class SSAConverter:
    def __init__(self):
        self.counter = 0
        self.var_map = {}

    def new_var(self, var_name):
        self.counter += 1
        return f"{var_name}_{self.counter}"

    def convert(self, node):
        if isinstance(node, Program):
            return Program([self.convert(stmt) for stmt in node.statements])
        elif isinstance(node, Assign):
            new_var_name = self.new_var(node.var.name)
            self.var_map[node.var.name] = new_var_name
            new_expr = self.convert_expr(node.expr)
            return Assign(Var(new_var_name), new_expr)
        elif isinstance(node, BinOp):
            left = self.convert_expr(node.left)
            right = self.convert_expr(node.right)
            return BinOp(left, node.op, right)
        elif isinstance(node, Var):
            return Var(self.var_map.get(node.name, node.name))
        return node

    def convert_expr(self, expr):
        if isinstance(expr, Var):
            return Var(self.var_map.get(expr.name, expr.name))
        elif isinstance(expr, BinOp):
            return BinOp(self.convert_expr(expr.left), expr.op, self.convert_expr(expr.right))
        return expr

def main():
    code = """
    x = 1
    y = x + 2
    z = y + 3
    """
    
    # Parse the code into an AST
    program = parse(code)
    
    # Convert the AST to SSA form
    ssa_converter = SSAConverter()
    ssa_program = ssa_converter.convert(program)
    
    # Print the SSA form
    for stmt in ssa_program.statements:
        if isinstance(stmt, Assign):
            print(f"{stmt.var.name} = {stmt.expr.left.name} {stmt.expr.op} {stmt.expr.right.name}")

if __name__ == "__main__":
    main()
