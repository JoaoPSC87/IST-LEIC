// cadeia de if-elif-else e instrucoes stop/next
begin
  int x = 5;
  if (x < 0)
    "negativo" !!
  elif (x == 0)
    "zero" !!
  elif (x < 10)
    "pequeno" !!
  else
    "grande" !!

  while (x > 0) {
    if (x == 3) stop;
    if (x == 7) next;
    x = x - 1;
  }
  return 0;
end
