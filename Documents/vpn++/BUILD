cc_binary(
    name = "vpn_app",
    srcs = ["src/main.cpp"],
    deps = [
        "//src/core:core",
        "@boost.asio//:boost.asio"
    ],
)

cc_binary(
    name = "test_client_spammer",
    srcs = ["test/test_client_spammer.cpp"],
    deps = ["@boost.asio//:boost.asio"]
)