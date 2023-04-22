package main

import (
	"context"
	"time"

	"github.com/cloudwego/netpoll"
)

func main() {
	network, address := "tcp", ":9999"
	listener, err := netpoll.CreateListener(network, address)
	if err != nil {
		panic(err)
	}

	eventLoop, err := netpoll.NewEventLoop(
		handle,
		netpoll.WithOnPrepare(prepare),
		netpoll.WithReadTimeout(time.Second),
	)
	if err != nil {
		panic(err)
	}

	// start listen loop ...
	err = eventLoop.Serve(listener)
	if err != nil {
		panic(err)
	}
}

var _ netpoll.OnPrepare = prepare
var _ netpoll.OnRequest = handle

func prepare(connection netpoll.Connection) context.Context {
	return context.Background()
}

func handle(ctx context.Context, connection netpoll.Connection) error {
	reader := connection.Reader()
	defer reader.Release()
	msg, _ := reader.ReadString(reader.Len())
	bytes := []byte(msg)
	writeToBytes := len(msg)
	writtenBytes := 0
	for {
		n, err := connection.Write(bytes)
		if err != nil {
			return err
		}
		writtenBytes += n
		if writtenBytes == writeToBytes {
			break
		}
	}
	return nil
}
