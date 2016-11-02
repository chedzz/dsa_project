 Encrption-Decryption using RSA algorithm
 Namyata Cheduri
 111503047

 The project implements (asynnetric) RSA algorithm to encrypt and decrypt the messages.
 1. Selecting two random prime numbers of approximately same range(p and q). Miller- Rabin primality test is used to
    check whether number is prime or not.
 2. calculating their product(n = p * q).
 3. calculating 'e' such that 1 < e < totient(n) and 'e' and 'n' are coprime, where totient(n) = (p - 1) * (q - 1).
 4. calculating 'd' such that d*e(mod n) = 1. 'd' is caalculated using 'extended euclidean algorithm'. 
 5. Encrypt using public key (e,n). If m is the message then cipher text 'c' is
              c = m^e(mod n).
 6. Decrypt using the private key(d, n). The message can be retrieved if cioher text given is c
              m(retireved) = c^d(mod n).

   To handle the large exponentiation(to avoid overflow) Divide and Conquer algorithm(Karatsuba) is used and to make it faster 
   the mod operation is done simultaneously.
   The data structure used is array(to perform bignumber calculations).
   Two structures and files are used.
   The program if used in -e option encrypts the data in source file and writes to the destination file.
   If run in -d option decrypts the cipher in source file and writes in the destination file.
   The matching of private and public key is done internally(in case program is run in -e option more than one time or vice versa).
