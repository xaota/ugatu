def nonzeros(m, row):
    for index in xrange(m.indptr[row], m.indptr[row+1]):
        yield m.indices[index], m.data[index]


def implicit_als_cg(Cui, features=20, iterations=20, lambda_val=0.1):
    user_size, item_size = Cui.shape

    X = np.random.rand(user_size, features) * 0.01
    Y = np.random.rand(item_size, features) * 0.01

    Cui, Ciu = Cui.tocsr(), Cui.T.tocsr()

    for iteration in xrange(iterations):
        print 'iteration %d of %d' % (iteration+1, iterations)
        least_squares_cg(Cui, X, Y, lambda_val)
        least_squares_cg(Ciu, Y, X, lambda_val)

    return sparse.csr_matrix(X), sparse.csr_matrix(Y)


  def least_squares_cg(Cui, X, Y, lambda_val, cg_steps=3):
    users, features = X.shape

    YtY = Y.T.dot(Y) + lambda_val * np.eye(features)

    for u in xrange(users):

        x = X[u]
        r = -YtY.dot(x)

        for i, confidence in nonzeros(Cui, u):
            r += (confidence - (confidence - 1) * Y[i].dot(x)) * Y[i]

        p = r.copy()
        rsold = r.dot(r)

        for it in xrange(cg_steps):
            Ap = YtY.dot(p)
            for i, confidence in nonzeros(Cui, u):
                Ap += (confidence - 1) * Y[i].dot(p) * Y[i]

            alpha = rsold / p.dot(Ap)
            x += alpha * p
            r -= alpha * Ap

            rsnew = r.dot(r)
            p = r + (rsnew / rsold) * p
            rsold = rsnew

        X[u] = x

alpha_val = 15
conf_data = (data_sparse * alpha_val).astype('double')
user_vecs, item_vecs = implicit_als_cg(conf_data, iterations=20, features=20)
