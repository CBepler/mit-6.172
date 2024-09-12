int fibonacci(int n) {
    if(n < 2) {
        return n;
    }
    int x = fibonacci(n - 1);
    int y = fibonacci(n - 2);
    return x + y;
}