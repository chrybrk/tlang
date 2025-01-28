import ast

class MyParser:
    def parse(self, code):
        return ast.parse(code)

class SSAConversion:
    def __init__(self):
        self.variable_counter = 0
        self.var_map = {}

    def rename_variable(self, var):
        if var not in self.var_map:
            self.var_map[var] = f"{var}{self.variable_counter}"
            self.variable_counter += 1
        return self.var_map[var]

    def convert_to_ssa(self, node):
        if isinstance(node, ast.Assign):
            target = node.targets[0].id
            new_var = self.rename_variable(target)
            # Create a new assignment with the renamed variable
            return ast.Assign([ast.Name(id=new_var, ctx=ast.Store())], node.value)

        elif isinstance(node, ast.Name):
            # Rename the variable if it's used in an expression
            return ast.Name(id=self.rename_variable(node.id), ctx=node.ctx)
        
        elif isinstance(node, ast.BinOp):
            # Recursively process the left and right operands
            node.left = self.convert_to_ssa(node.left)
            node.right = self.convert_to_ssa(node.right)
            return node

        return node

class ConstantPropagation:
    def __init__(self):
        self.constants = {}

    def propagate_constants(self, node):
        if isinstance(node, ast.Assign):
            if isinstance(node.value, ast.Constant):
                self.constants[node.targets[0].id] = node.value.value
            return node
        elif isinstance(node, ast.Name):
            if node.id in self.constants:
                return ast.Constant(value=self.constants[node.id])
        return node

class SSAOptimizer:
    def __init__(self):
        self.ssa_converter = SSAConversion()
        self.constant_propagation = ConstantPropagation()

    def optimize(self, code):
        parsed_code = ast.parse(code)
        ssa_code = self.ssa_converter.convert_to_ssa(parsed_code)
        optimized_code = self.constant_propagation.propagate_constants(ssa_code)
        return optimized_code


# Example usage:
optimizer = SSAOptimizer()
optimized_code = optimizer.optimize("""
x = 1
y = 1
y = x
""")
print(ast.dump(optimized_code))
