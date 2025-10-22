import os
import subprocess
import argparse
import sys

class Colors:
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'

def run_tests(emulator_path, tests_dir):
    if not os.path.isfile(emulator_path):
        print(f"{Colors.RED}Error: Emulator not found at '{emulator_path}'{Colors.ENDC}")
        sys.exit(1)

    if not os.path.isdir(tests_dir):
        print(f"{Colors.RED}Error: Test directory not found at '{tests_dir}'{Colors.ENDC}")
        sys.exit(1)

    print(f"{Colors.BOLD}Starting RISC-V tests...{Colors.ENDC}")
    print(f"Emulator: {emulator_path}")
    print(f"Test Directory: {tests_dir}\n")

    passed_count = 0
    failed_count = 0
    # test_files = sorted([f for f in os.listdir(tests_dir) if os.path.isfile(os.path.join(tests_dir, f))])
    test_files = []
    for file in os.listdir(tests_dir):
        # REMOVE THIS LATER ON ME
        if "-v-" in file:
            continue

        test_path = os.path.join(tests_dir, file)
        if not os.path.isfile(test_path):
            continue

        with open(test_path, "rb") as f:
            if f.read(4) == b'\x7fELF':
                test_files.append(file)

    test_files = sorted(test_files)
    total_tests = len(test_files)

    if total_tests == 0:
        print(f"{Colors.YELLOW}Warning: No test files found in '{tests_dir}'.{Colors.ENDC}")
        print("Make sure you are pointing to the correct 'isa' directory from riscv-tests.")
        return

    for i, test_name in enumerate(test_files):
        test_path = os.path.join(tests_dir, test_name)
        command = [emulator_path, "--test" , test_path]

        try:
            result = subprocess.run(
                command,
                capture_output=False,
                text=True,
                timeout=5
            )

            if result.returncode == 0:
                passed_count += 1
                print(f"[{i+1:>{len(str(total_tests))}}/{total_tests}] {Colors.GREEN}PASSED{Colors.ENDC} : {test_name}")
            else:
                failed_count += 1
                print(f"[{i+1:>{len(str(total_tests))}}/{total_tests}] {Colors.RED}FAILED (case {result.returncode}){Colors.ENDC} : {test_name}")
                # Print stderr for debugging failed tests
                if result.stderr:
                    print(result.stderr.strip())

        except FileNotFoundError:
            print(f"{Colors.RED}Error: Command not found. Is '{emulator_path}' correct?{Colors.ENDC}")
            sys.exit(1)
        except subprocess.TimeoutExpired:
            failed_count += 1
            print(f"[{i+1:>{len(str(total_tests))}}/{total_tests}] {Colors.RED}TIMEOUT{Colors.ENDC}: {test_name}")
        except Exception as e:
            failed_count += 1
            print(f"[{i+1:>{len(str(total_tests))}}/{Colors.RED}CRASHED{Colors.ENDC}: {test_name} ({e})")

    # --- Print Summary ---
    print("\n" + "="*20)
    print(f"{Colors.BOLD}Test Summary{Colors.ENDC}")
    print("="*20)
    print(f"{Colors.GREEN}Passed: {passed_count}{Colors.ENDC}")
    print(f"{Colors.RED}Failed: {failed_count}{Colors.ENDC}")
    print(f"Total:  {total_tests}")
    print("="*20)

    if failed_count > 0:
        sys.exit(1)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Run RISC-V ISA tests against a specified emulator.",
        formatter_class=argparse.RawTextHelpFormatter
    )
    parser.add_argument(
        "emulator",
        help="Path to the RISC-V emulator executable."
    )
    parser.add_argument(
        "tests_dir",
        help="Path to the directory containing the compiled riscv-tests (e.g., .../riscv-tests/isa)."
    )
    args = parser.parse_args()

    run_tests(args.emulator, args.tests_dir)
