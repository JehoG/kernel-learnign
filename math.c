int mod(int number, int mod){
    while(number >= mod){
        number -= mod;
    }
    return number;
}

int div(int number, int div){
    int rest = number;
    int result = 0;
    while(rest > div){
        rest -= div;
        result++;
    }
    return result;
}

