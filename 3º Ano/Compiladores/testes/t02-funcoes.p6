// definicao de funcoes com e sem argumentos
public factorial(int n) -> int {
  if (n > 1)
    return n * factorial(n - 1);
  else
    return 1;
};

soma(int a, int b) -> int {
  return a + b;
};

reset() -> void {
};
