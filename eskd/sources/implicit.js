function implicit(matrix) {
  const confidence = matrix.scalar(alpha);

  let X   = Matrix.random(users,   features);
  let Y   = Matrix.random(stories, features);
  let X_I = Matrix.identity(users);
  let Y_I = Matrix.identity(stories);
  let I   = Matrix.identity(features);
  let lI  = I.scalar(lambda);

  for (let k = 0; k < iterations; ++k) {
    const yTy = Y.transpose().multiply(Y);
    const xTx = X.transpose().multiply(X);

    for (let u = 0; u < users; ++u) {
      let u_row      = confidence.row(u);
      const p_u      = u_row.copy();
      p_u.data       = p_u.data.map(e => e !== 0 ? 1 : 0);
      const CuI      = Matrix.diagonal(u_row);
      const Cu       = CuI.addition(Y_I);
      const yT_CuI_y = Y.transpose().multiply(CuI).multiply(Y)
      const tempV    = Matrix.from(p_u);
      const tempM    = Y.transpose().multiply(Cu);
      const yT_Cu_pu = tempM.multiply(tempV).col(0);
      const A        = yTy.addition(yT_CuI_y).addition(lI);
      X              = X.setRow(u, A.solve(yT_Cu_pu));
    }

    for (let i = 0; i < stories; ++i) {
      const i_row    = confidence.col(i);
      const p_i      = i_row.copy()
      p_i.data       = p_i.data.map(e => e !== 0 ? 1 : 0);
      const CiI      = Matrix.diagonal(i_row);
      const Ci       = CiI.addition(X_I);
      const xT_CiI_x = X.transpose().multiply(CiI).multiply(X)
      const tempV    = Matrix.from(p_i);
      const tempM    = X.transpose().multiply(Ci);
      let xT_Ci_pi   = tempM.multiply(tempV).col(0);
      const A        = xTx.addition(xT_CiI_x).addition(lI);
      Y              = Y.setRow(i, A.solve(xT_Ci_pi));
    }
  }
  return {X, Y}
}
