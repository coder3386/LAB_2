use std::io;

// 행렬 입력 함수
fn input_matrix(rows: usize, cols: usize) -> Vec<Vec<i32>> {
    let mut matrix = vec![vec![0; cols]; rows];

    println!("행렬의 원소를 입력하세요 ({}x{}):", rows, cols);
    for i in 0..rows {
        for j in 0..cols {
            println!("행렬 ({}X{}) ", i, j);
            let mut input = String::new();
            io::stdin().read_line(&mut input).unwrap();
            matrix[i][j] = input.trim().parse().unwrap();
        }
    }
    matrix
}

// 행렬 덧셈 함수
fn add_matrices(a: &Vec<Vec<i32>>, b: &Vec<Vec<i32>>) -> Vec<Vec<i32>> {
    let rows = a.len();
    let cols = a[0].len();
    let mut result = vec![vec![0; cols]; rows];

    for i in 0..rows {
        for j in 0..cols {
            result[i][j] = a[i][j] + b[i][j];
        }
    }
    result
}

fn main() {
    let mut input = String::new();

    // 행 크기 입력
    println!("행렬의 행(rows) 수를 입력하세요:");
    io::stdin().read_line(&mut input).unwrap();
    let rows: usize = input.trim().parse().unwrap();
    input.clear();

    // 열 크기 입력
    println!("행렬의 열(cols) 수를 입력하세요:");
    io::stdin().read_line(&mut input).unwrap();
    let cols: usize = input.trim().parse().unwrap();

    // 두 행렬 입력
    println!("\n=== 첫 번째 행렬 입력 ===");
    let matrix1 = input_matrix(rows, cols);

    println!("\n=== 두 번째 행렬 입력 ===");
    let matrix2 = input_matrix(rows, cols);

    // 행렬 덧셈 수행
    let result = add_matrices(&matrix1, &matrix2);

    // 출력
    println!("\n=== 결과 행렬 ===");
    for row in result {
        for val in row {
            print!("{:4} ", val);
        }
        println!();
    }
}

