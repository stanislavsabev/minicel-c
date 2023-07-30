# Coding exercise: Implement mini Excel-like engine in C

Read .csv file.

Parse values and evaluate the expressions.

Report circular references and other errors.

## Example

Input looks like this...

```csv
A      , B
1      , 2
3      , 4
=A1+B1 , =A2+B2
```
..and the output like this

```csv
A      | B
1      | 2
3      | 4
3      | 7
```

## Credits

Checkout the original inspiration at https://github.com/tsoding/minicel