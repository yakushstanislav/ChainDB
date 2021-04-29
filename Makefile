# Copyright (c) 2021 Stanislav Yakush (st.yakush@yandex.ru)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

TARGET = ChainDB
BUILD_DIR = build

NPROC = $(shell nproc)

CONTAINER = chain-database-dev-env
CONTAINER_DIR = chain-database

TARGET_RUN_CMD = ./$(BUILD_DIR)/$(TARGET) --daemonize false --log-path $(BUILD_DIR)/chain_database.log --storage-path ${BUILD_DIR}

TARGET_ARTIFACTS = ChainDB cli/cli tests/runtest

.PHONY: env all run gdb valgrind callgrind cppcheck test pack shell clean
.DEFAULT_GOAL := all

define run-env
	docker run -it -v $(PWD):/${CONTAINER_DIR} -p 8888:8888 --rm ${CONTAINER} /bin/sh -c $(1)
endef

env:
	@echo "Prepare docker build & testing environment..."
	@docker build -t ${CONTAINER} .
	@echo "Done!"

all: env
	@echo "Build (debug mode)..."
	$(call run-env, "mkdir -p ${BUILD_DIR} && cd ${BUILD_DIR} && cmake .. -DBUILD_DEBUG=ON -DBUILD_CLI=ON -DBUILD_TESTS=ON && make -j${NPROC}")
	@echo "Done!"

release: env
	@echo "Build (release mode)..."
	$(call run-env, "mkdir -p ${BUILD_DIR} && cd ${BUILD_DIR} && cmake .. -DBUILD_CLI=ON -DBUILD_TESTS=ON && make -j${NPROC}")
	@echo "Done!"

run: env
	@echo "Run..."
	$(call run-env, "${TARGET_RUN_CMD}")

gdb: env
	@echo "Run in GDB..."
	$(call run-env, "gdb -ex run --args ${TARGET_RUN_CMD}")

valgrind: env
	@echo "Run in Valgrind..."
	$(call run-env, "valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ${TARGET_RUN_CMD}")

callgrind: env
	@echo "Run in Callgrind..."
	$(call run-env, "valgrind --tool=callgrind --dump-instr=yes --simulate-cache=yes --collect-jumps=yes ${TARGET_RUN_CMD}")

cppcheck: env
	@echo "Run Cppcheck..."
	$(call run-env, "cppcheck -q --enable=all --inconclusive -I include/ src/")

test: env
	@echo "Run tests..."
	$(call run-env, "./$(BUILD_DIR)/tests/runtest")
	@echo "Done!"

pack: env
	@echo "Pack..."
	$(call run-env, "rm -f ${BUILD_DIR}/*.tar && tar -C ${BUILD_DIR} -cf ${BUILD_DIR}/${TARGET}_`date +%s`.tar ${TARGET_ARTIFACTS}")
	@echo "Done!"

shell: env
	@echo "Run shell..."
	@docker run -it -v $(PWD):/${CONTAINER_DIR} --network="host" --rm ${CONTAINER}

clean: env
	@echo "Clean..."
	$(call run-env, "rm -rf $(BUILD_DIR)")
	@echo "Done!"