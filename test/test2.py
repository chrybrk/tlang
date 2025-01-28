import ast
from collections import defaultdict

class SSAOptimization:
    def __init__(self):
        self.variable_counter = 0
        self.var_map = {}  # Map original variables to SSA variables
        self.live_vars = set()  # Set of variables that are still used
        self.used_expressions = defaultdict(list)  # Track used expressions for CSE
        self.constants = {}  # Track constant values for Constant Folding

    # Step 1: Convert to SSA form by renaming variables
    def rename_variable(self, var):
        if var not in self.var_map:
            self.var_map[var] = f"{var}_{self.variable_counter}"
            self.variable_counter += 1
        return self.var_map[var]

    def convert_to_ssa(self, node):
        if isinstance(node, ast.Assign):
            target = node.targets[0].id
            new_var = self.rename_variable(target)
            node.targets[0].id = new_var
            # Add the assignment to live variables
            self.live_vars.add(new_var)
            node.value = self.convert_to_ssa(node.value)
            return node
        elif isinstance(node, ast.Name):
            return ast.Name(id=self.rename_variable(node.id), ctx=node.ctx)
        elif isinstance(node, ast.BinOp):
            node.left = self.convert_to_ssa(node.left)
            node.right = self.convert_to_ssa(node.right)
            return node
        elif isinstance(node, ast.Constant):
            return node
        return node

    # Step 2: Constant Folding (Evaluating constant expressions)
    def fold_constants(self, node):
        if isinstance(node, ast.BinOp):
            left = self.fold_constants(node.left)
            right = self.fold_constants(node.right)
            if isinstance(left, ast.Constant) and isinstance(right, ast.Constant):
                # Perform constant folding
                if isinstance(node.op, ast.Add):
                    return ast.Constant(left.value + right.value)
                elif isinstance(node.op, ast.Sub):
                    return ast.Constant(left.value - right.value)
                elif isinstance(node.op, ast.Mult):
                    return ast.Constant(left.value * right.value)
                elif isinstance(node.op, ast.Div):
                    return ast.Constant(left.value / right.value)
            node.left = left
            node.right = right
            return node
        return node

    # Step 3: Common Subexpression Elimination (CSE)
    def eliminate_common_subexpressions(self, node):
        if isinstance(node, ast.BinOp):
            left = self.eliminate_common_subexpressions(node.left)
            right = self.eliminate_common_subexpressions(node.right)
            expr = ast.dump(left) + str(node.op) + ast.dump(right)
            if expr in self.used_expressions:
                # If the expression has been computed before, use the result
                return self.used_expressions[expr]
            else:
                # Store the result of this expression for future reuse
                new_var = self.rename_variable(f"tmp_{len(self.used_expressions)}")
                tmp_assign = ast.Assign([ast.Name(id=new_var, ctx=ast.Store())], node)
                self.used_expressions[expr] = ast.Name(id=new_var, ctx=ast.Load())
                return self.used_expressions[expr]
            node.left = left
            node.right = right
            return node
        return node

    # Step 4: Dead Code Elimination (DCE)
    def eliminate_dead_code(self, node):
        if isinstance(node, ast.Assign):
            target = node.targets[0].id
            # If the variable is not live, eliminate the assignment
            if target not in self.live_vars:
                return None  # Dead code, remove this assignment
            else:
                self.live_vars.add(target)
            node.value = self.eliminate_dead_code(node.value)
            return node
        elif isinstance(node, ast.Name):
            return node
        elif isinstance(node, ast.BinOp):
            node.left = self.eliminate_dead_code(node.left)
            node.right = self.eliminate_dead_code(node.right)
            return node
        return node

    def optimize(self, code):
        # Parse the code into an AST
        parsed_code = ast.parse(code)

        # Process each statement in the 'body' of the module
        optimized_body = []
        for stmt in parsed_code.body:
            stmt = self.convert_to_ssa(stmt)  # Step 1: Convert to SSA
            stmt = self.fold_constants(stmt)  # Step 2: Constant Folding
            stmt = self.eliminate_common_subexpressions(stmt)  # Step 3: CSE
            stmt = self.eliminate_dead_code(stmt)  # Step 4: DCE

            if stmt:  # Only add the statement if it's not dead code
                optimized_body.append(stmt)

        # Rebuild the module with the optimized body
        parsed_code.body = optimized_body
        return parsed_code


# Example usage
optimizer = SSAOptimization()

code = """
x = 1
y = 2
z = x + y
a = x + y
b = z + a
c = 1 + 2  # constant folding
"""

# Perform optimization
optimized_code = optimizer.optimize(code)

# Display the optimized AST (for inspection)
print(ast.dump(optimized_code, indent=4))
